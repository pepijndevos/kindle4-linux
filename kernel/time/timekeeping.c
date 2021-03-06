/*
 *  linux/kernel/time/timekeeping.c
 *
 *  Kernel timekeeping code and accessor functions
 *
 *  This code was moved from linux/kernel/timer.c.
 *  Please see that file for copyright and history logs.
 *
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/percpu.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sysdev.h>
#include <linux/clocksource.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/tick.h>


/*
 * This read-write spinlock protects us from races in SMP while
 * playing with xtime.
 */
__cacheline_aligned_in_smp DEFINE_ATOMIC_SEQLOCK(xtime_lock);

/*
 * The current time
 * wall_to_monotonic is what we need to add to xtime (or xtime corrected
 * for sub jiffie times) to get to monotonic time.  Monotonic is pegged
 * at zero at system boot time, so wall_to_monotonic will be negative,
 * however, we will ALWAYS keep the tv_nsec part positive so we can use
 * the usual normalization.
 *
 * wall_to_monotonic is moved after resume from suspend for the monotonic
 * time not to jump. We need to add total_sleep_time to wall_to_monotonic
 * to get the real boot based time offset.
 *
 * - wall_to_monotonic is no longer the boot time, getboottime must be
 * used instead.
 */
struct timespec xtime __attribute__ ((aligned (16)));
struct timespec wall_to_monotonic __attribute__ ((aligned (16)));
static unsigned long total_sleep_time;		/* seconds */

/* flag for if timekeeping is suspended */
int __read_mostly timekeeping_suspended;

struct clocksource *clock;

#ifdef CONFIG_GENERIC_TIME
/**
 * clocksource_forward_now - update clock to the current time
 *
 * Forward the current clock to update its state since the last call to
 * update_wall_time(). This is useful before significant clock changes,
 * as it avoids having to deal with this time offset explicitly.
 */
static void clocksource_forward_now(void)
{
	cycle_t cycle_now, cycle_delta;
	s64 nsec;

	cycle_now = clocksource_read(clock);
	cycle_delta = (cycle_now - clock->cycle_last) & clock->mask;
	clock->cycle_last = cycle_now;

	nsec = cyc2ns(clock, cycle_delta);

	/* If arch requires, add in gettimeoffset() */
	nsec += arch_gettimeoffset();

	timespec_add_ns(&xtime, nsec);

	nsec = ((s64)cycle_delta * clock->mult_orig) >> clock->shift;
	clock->raw_time.tv_nsec += nsec;
}

/**
 * getnstimeofday - Returns the time of day in a timespec
 * @ts:		pointer to the timespec to be set
 *
 * Returns the time of day in a timespec.
 */
void getnstimeofday(struct timespec *ts)
{
	cycle_t cycle_now, cycle_delta;
	unsigned long seq;
	s64 nsecs;

	WARN_ON(timekeeping_suspended);

	do {
		seq = read_atomic_seqbegin(&xtime_lock);

		*ts = xtime;

		/* read clocksource: */
		cycle_now = clocksource_read(clock);

		/* calculate the delta since the last update_wall_time: */
		cycle_delta = (cycle_now - clock->cycle_last) & clock->mask;

		/* convert to nanoseconds: */
		nsecs = cyc2ns(clock, cycle_delta);

		/* If arch requires, add in gettimeoffset() */
		nsecs += arch_gettimeoffset();

	} while (read_atomic_seqretry(&xtime_lock, seq));

	timespec_add_ns(ts, nsecs);
}

EXPORT_SYMBOL(getnstimeofday);

ktime_t ktime_get(void)
{
	cycle_t cycle_now, cycle_delta;
	unsigned int seq;
	s64 secs, nsecs;

	WARN_ON(timekeeping_suspended);

	do {
		seq = read_atomic_seqbegin(&xtime_lock);
		secs = xtime.tv_sec + wall_to_monotonic.tv_sec;
		nsecs = xtime.tv_nsec + wall_to_monotonic.tv_nsec;

		/* read clocksource: */
		cycle_now = clocksource_read(clock);

		/* calculate the delta since the last update_wall_time: */
		cycle_delta = (cycle_now - clock->cycle_last) & clock->mask;

		/* convert to nanoseconds: */
		nsecs += cyc2ns(clock, cycle_delta);

	} while (read_atomic_seqretry(&xtime_lock, seq));
	/*
	 * Use ktime_set/ktime_add_ns to create a proper ktime on
	 * 32-bit architectures without CONFIG_KTIME_SCALAR.
	 */
	return ktime_add_ns(ktime_set(secs, 0), nsecs);
}
EXPORT_SYMBOL_GPL(ktime_get);

/**
 * ktime_get_ts - get the monotonic clock in timespec format
 * @ts:		pointer to timespec variable
 *
 * The function calculates the monotonic clock from the realtime
 * clock and the wall_to_monotonic offset and stores the result
 * in normalized timespec format in the variable pointed to by @ts.
 */
void ktime_get_ts(struct timespec *ts)
{
	cycle_t cycle_now, cycle_delta;
	struct timespec tomono;
	unsigned int seq;
	s64 nsecs;

	WARN_ON(timekeeping_suspended);

	do {
		seq = read_atomic_seqbegin(&xtime_lock);
		*ts = xtime;
		tomono = wall_to_monotonic;

		/* read clocksource: */
		cycle_now = clocksource_read(clock);

		/* calculate the delta since the last update_wall_time: */
		cycle_delta = (cycle_now - clock->cycle_last) & clock->mask;

		/* convert to nanoseconds: */
		nsecs = cyc2ns(clock, cycle_delta);

	} while (read_atomic_seqretry(&xtime_lock, seq));

	set_normalized_timespec(ts, ts->tv_sec + tomono.tv_sec,
				ts->tv_nsec + tomono.tv_nsec + nsecs);
}
EXPORT_SYMBOL_GPL(ktime_get_ts);

/**
 * do_gettimeofday - Returns the time of day in a timeval
 * @tv:		pointer to the timeval to be set
 *
 * NOTE: Users should be converted to using getnstimeofday()
 */
void do_gettimeofday(struct timeval *tv)
{
	struct timespec now;

	getnstimeofday(&now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec/1000;
}

EXPORT_SYMBOL(do_gettimeofday);
/**
 * do_settimeofday - Sets the time of day
 * @tv:		pointer to the timespec variable containing the new time
 *
 * Sets the time of day to the new time and update NTP and notify hrtimers
 */
int do_settimeofday(struct timespec *tv)
{
	struct timespec ts_delta;
	unsigned long flags;

	if ((unsigned long)tv->tv_nsec >= NSEC_PER_SEC)
		return -EINVAL;

	write_atomic_seqlock_irqsave(&xtime_lock, flags);

	clocksource_forward_now();

	ts_delta.tv_sec = tv->tv_sec - xtime.tv_sec;
	ts_delta.tv_nsec = tv->tv_nsec - xtime.tv_nsec;
	wall_to_monotonic = timespec_sub(wall_to_monotonic, ts_delta);

	xtime = *tv;

	clock->error = 0;
	ntp_clear();

	update_vsyscall(&xtime, clock);

	write_atomic_sequnlock_irqrestore(&xtime_lock, flags);

	/* signal hrtimers about time change */
	clock_was_set();

	return 0;
}

EXPORT_SYMBOL(do_settimeofday);

/**
 * change_clocksource - Swaps clocksources if a new one is available
 *
 * Accumulates current time interval and initializes new clocksource
 */
static void change_clocksource(void)
{
	struct clocksource *new, *old;

	new = clocksource_get_next();

	if (clock == new)
		return;

	clocksource_forward_now();

	if (clocksource_enable(new))
		return;

	new->raw_time = clock->raw_time;
	old = clock;
	clock = new;
	clocksource_disable(old);

	clock->cycle_last = 0;
	clock->cycle_last = clocksource_read(clock);
	clock->error = 0;
	clock->xtime_nsec = 0;
	clocksource_calculate_interval(clock, NTP_INTERVAL_LENGTH);

	tick_clock_notify();

	/*
	 * We're holding xtime lock and waking up klogd would deadlock
	 * us on enqueue.  So no printing!
	printk(KERN_INFO "Time: %s clocksource has been installed.\n",
	       clock->name);
	 */
}
#else /* GENERIC_TIME */
static inline void clocksource_forward_now(void) { }
static inline void change_clocksource(void) { }

/**
 * ktime_get - get the monotonic time in ktime_t format
 *
 * returns the time in ktime_t format
 */
ktime_t ktime_get(void)
{
	struct timespec now;

	ktime_get_ts(&now);

	return timespec_to_ktime(now);
}
EXPORT_SYMBOL_GPL(ktime_get);

/**
 * ktime_get_ts - get the monotonic clock in timespec format
 * @ts:		pointer to timespec variable
 *
 * The function calculates the monotonic clock from the realtime
 * clock and the wall_to_monotonic offset and stores the result
 * in normalized timespec format in the variable pointed to by @ts.
 */
void ktime_get_ts(struct timespec *ts)
{
	struct timespec tomono;
	unsigned long seq;

	do {
		seq = read_atomic_seqbegin(&xtime_lock);
		getnstimeofday(ts);
		tomono = wall_to_monotonic;

	} while (read_atomic_seqretry(&xtime_lock, seq));

	set_normalized_timespec(ts, ts->tv_sec + tomono.tv_sec,
				ts->tv_nsec + tomono.tv_nsec);
}
EXPORT_SYMBOL_GPL(ktime_get_ts);
#endif /* !GENERIC_TIME */

/**
 * ktime_get_real - get the real (wall-) time in ktime_t format
 *
 * returns the time in ktime_t format
 */
ktime_t ktime_get_real(void)
{
	struct timespec now;

	getnstimeofday(&now);

	return timespec_to_ktime(now);
}
EXPORT_SYMBOL_GPL(ktime_get_real);

/**
 * getrawmonotonic - Returns the raw monotonic time in a timespec
 * @ts:		pointer to the timespec to be set
 *
 * Returns the raw monotonic time (completely un-modified by ntp)
 */
void getrawmonotonic(struct timespec *ts)
{
	unsigned long seq;
	s64 nsecs;
	cycle_t cycle_now, cycle_delta;

	do {
		seq = read_atomic_seqbegin(&xtime_lock);

		/* read clocksource: */
		cycle_now = clocksource_read(clock);

		/* calculate the delta since the last update_wall_time: */
		cycle_delta = (cycle_now - clock->cycle_last) & clock->mask;

		/* convert to nanoseconds: */
		nsecs = ((s64)cycle_delta * clock->mult_orig) >> clock->shift;

		*ts = clock->raw_time;

	} while (read_atomic_seqretry(&xtime_lock, seq));

	timespec_add_ns(ts, nsecs);
}
EXPORT_SYMBOL(getrawmonotonic);


/**
 * timekeeping_valid_for_hres - Check if timekeeping is suitable for hres
 */
int timekeeping_valid_for_hres(void)
{
	unsigned long seq;
	int ret;

	do {
		seq = read_atomic_seqbegin(&xtime_lock);

		ret = clock->flags & CLOCK_SOURCE_VALID_FOR_HRES;

	} while (read_atomic_seqretry(&xtime_lock, seq));

	return ret;
}

/**
 * read_persistent_clock -  Return time in seconds from the persistent clock.
 *
 * Weak dummy function for arches that do not yet support it.
 * Returns seconds from epoch using the battery backed persistent clock.
 * Returns zero if unsupported.
 *
 *  XXX - Do be sure to remove it once all arches implement it.
 */
unsigned long __attribute__((weak)) read_persistent_clock(void)
{
	return 0;
}

/*
 * timekeeping_init - Initializes the clocksource and common timekeeping values
 */
void __init timekeeping_init(void)
{
	unsigned long flags;
	unsigned long sec = read_persistent_clock();

	write_atomic_seqlock_irqsave(&xtime_lock, flags);

	ntp_init();

	clock = clocksource_get_next();
	clocksource_enable(clock);
	clocksource_calculate_interval(clock, NTP_INTERVAL_LENGTH);
	clock->cycle_last = clocksource_read(clock);

	xtime.tv_sec = sec;
	xtime.tv_nsec = 0;
	set_normalized_timespec(&wall_to_monotonic,
				-xtime.tv_sec, -xtime.tv_nsec);
	total_sleep_time = 0;
	write_atomic_sequnlock_irqrestore(&xtime_lock, flags);
}

/* time in seconds when suspend began */
static unsigned long timekeeping_suspend_time;

/**
 * timekeeping_resume - Resumes the generic timekeeping subsystem.
 * @dev:	unused
 *
 * This is for the generic clocksource timekeeping.
 * xtime/wall_to_monotonic/jiffies/etc are
 * still managed by arch specific suspend/resume code.
 */
static int timekeeping_resume(struct sys_device *dev)
{
	unsigned long flags;
	unsigned long now = read_persistent_clock();

	clocksource_resume();

	write_atomic_seqlock_irqsave(&xtime_lock, flags);

	if (now && (now > timekeeping_suspend_time)) {
		unsigned long sleep_length = now - timekeeping_suspend_time;

		xtime.tv_sec += sleep_length;
		wall_to_monotonic.tv_sec -= sleep_length;
		total_sleep_time += sleep_length;
	}
	/* re-base the last cycle value */
	clock->cycle_last = 0;
	clock->cycle_last = clocksource_read(clock);
	clock->error = 0;
	timekeeping_suspended = 0;
	write_atomic_sequnlock_irqrestore(&xtime_lock, flags);

	touch_softlockup_watchdog();

	clockevents_notify(CLOCK_EVT_NOTIFY_RESUME, NULL);

	/* Resume hrtimers */
	hres_timers_resume();

	return 0;
}

static int timekeeping_suspend(struct sys_device *dev, pm_message_t state)
{
	unsigned long flags;

	timekeeping_suspend_time = read_persistent_clock();

	write_atomic_seqlock_irqsave(&xtime_lock, flags);
	clocksource_forward_now();
	timekeeping_suspended = 1;
	write_atomic_sequnlock_irqrestore(&xtime_lock, flags);

	clockevents_notify(CLOCK_EVT_NOTIFY_SUSPEND, NULL);

	return 0;
}

/* sysfs resume/suspend bits for timekeeping */
static struct sysdev_class timekeeping_sysclass = {
	.name		= "timekeeping",
	.resume		= timekeeping_resume,
	.suspend	= timekeeping_suspend,
};

static struct sys_device device_timer = {
	.id		= 0,
	.cls		= &timekeeping_sysclass,
};

static int __init timekeeping_init_device(void)
{
	int error = sysdev_class_register(&timekeeping_sysclass);
	if (!error)
		error = sysdev_register(&device_timer);
	return error;
}

device_initcall(timekeeping_init_device);

/*
 * If the error is already larger, we look ahead even further
 * to compensate for late or lost adjustments.
 */
static __always_inline int clocksource_bigadjust(s64 error, s64 *interval,
						 s64 *offset)
{
	s64 tick_error, i;
	u32 look_ahead, adj;
	s32 error2, mult;

	/*
	 * Use the current error value to determine how much to look ahead.
	 * The larger the error the slower we adjust for it to avoid problems
	 * with losing too many ticks, otherwise we would overadjust and
	 * produce an even larger error.  The smaller the adjustment the
	 * faster we try to adjust for it, as lost ticks can do less harm
	 * here.  This is tuned so that an error of about 1 msec is adjusted
	 * within about 1 sec (or 2^20 nsec in 2^SHIFT_HZ ticks).
	 */
	error2 = clock->error >> (NTP_SCALE_SHIFT + 22 - 2 * SHIFT_HZ);
	error2 = abs(error2);
	for (look_ahead = 0; error2 > 0; look_ahead++)
		error2 >>= 2;

	/*
	 * Now calculate the error in (1 << look_ahead) ticks, but first
	 * remove the single look ahead already included in the error.
	 */
	tick_error = tick_length >> (NTP_SCALE_SHIFT - clock->shift + 1);
	tick_error -= clock->xtime_interval >> 1;
	error = ((error - tick_error) >> look_ahead) + tick_error;

	/* Finally calculate the adjustment shift value.  */
	i = *interval;
	mult = 1;
	if (error < 0) {
		error = -error;
		*interval = -*interval;
		*offset = -*offset;
		mult = -1;
	}
	for (adj = 0; error > i; adj++)
		error >>= 1;

	*interval <<= adj;
	*offset <<= adj;
	return mult << adj;
}

/*
 * Adjust the multiplier to reduce the error value,
 * this is optimized for the most common adjustments of -1,0,1,
 * for other values we can do a bit more work.
 */
static void clocksource_adjust(s64 offset)
{
	s64 error, interval = clock->cycle_interval;
	int adj;

	error = clock->error >> (NTP_SCALE_SHIFT - clock->shift - 1);
	if (error > interval) {
		error >>= 2;
		if (likely(error <= interval))
			adj = 1;
		else
			adj = clocksource_bigadjust(error, &interval, &offset);
	} else if (error < -interval) {
		error >>= 2;
		if (likely(error >= -interval)) {
			adj = -1;
			interval = -interval;
			offset = -offset;
		} else
			adj = clocksource_bigadjust(error, &interval, &offset);
	} else
		return;

	clock->mult += adj;
	clock->xtime_interval += interval;
	clock->xtime_nsec -= offset;
	clock->error -= (interval - offset) <<
			(NTP_SCALE_SHIFT - clock->shift);
}

/**
 * logarithmic_accumulation - shifted accumulation of cycles
 *
 * This functions accumulates a shifted interval of cycles into
 * into a shifted interval nanoseconds. Allows for O(log) accumulation
 * loop.
 *
 * Returns the unconsumed cycles.
 */
static cycle_t logarithmic_accumulation(cycle_t offset, int shift)
{
	u64 nsecps = (u64)NSEC_PER_SEC << clock->shift;

	/* If the offset is smaller then a shifted interval, do nothing */
	if (offset < clock->cycle_interval<<shift)
		return offset;

	/* Accumulate one shifted interval */
	offset -= clock->cycle_interval << shift;
	clock->cycle_last += clock->cycle_interval << shift;

	clock->xtime_nsec += clock->xtime_interval << shift;
	while (clock->xtime_nsec >= nsecps) {
		clock->xtime_nsec -= nsecps;
		xtime.tv_sec++;
		second_overflow();
	}

	/* Accumulate into raw time */
	clock->raw_time.tv_nsec += clock->raw_interval << shift;;
	while (clock->raw_time.tv_nsec >= NSEC_PER_SEC) {
		clock->raw_time.tv_nsec -= NSEC_PER_SEC;
		clock->raw_time.tv_sec++;
	}

	/* Accumulate error between NTP and clock interval */
	clock->error += tick_length << shift;
	clock->error -= clock->xtime_interval <<
				(NTP_SCALE_SHIFT - clock->shift + shift);

	return offset;
}

/**
 * update_wall_time - Uses the current clocksource to increment the wall time
 *
 * Called from the timer interrupt, must hold a write on xtime_lock.
 */
void update_wall_time(void)
{
	cycle_t offset;
	int shift = 0, maxshift;

	/* Make sure we're fully resumed: */
	if (unlikely(timekeeping_suspended))
		return;

#ifdef CONFIG_GENERIC_TIME
	offset = (clocksource_read(clock) - clock->cycle_last) & clock->mask;
#else
	offset = clock->cycle_interval;
#endif
	clock->xtime_nsec = (s64)xtime.tv_nsec << clock->shift;

	/*
	 * With NO_HZ we may have to accumulate many cycle_intervals
	 * (think "ticks") worth of time at once. To do this efficiently,
	 * we calculate the largest doubling multiple of cycle_intervals
	 * that is smaller then the offset. We then accumulate that
	 * chunk in one go, and then try to consume the next smaller
	 * doubled multiple.
	 */
	shift = ilog2(offset) - ilog2(clock->cycle_interval);
	shift = max(0, shift);
	/* Bound shift to one less then what overflows tick_length */
	maxshift = (8*sizeof(tick_length) - (ilog2(tick_length)+1)) - 1;
	shift = min(shift, maxshift);
	while (offset >= clock->cycle_interval) {
		offset = logarithmic_accumulation(offset, shift);
		shift--;
	}

	/* correct the clock when NTP error is too big */
	clocksource_adjust(offset);

	/*
	 * Since in the loop above, we accumulate any amount of time
	 * in xtime_nsec over a second into xtime.tv_sec, its possible for
	 * xtime_nsec to be fairly small after the loop. Further, if we're
	 * slightly speeding the clocksource up in clocksource_adjust(),
	 * its possible the required corrective factor to xtime_nsec could
	 * cause it to underflow.
	 *
	 * Now, we cannot simply roll the accumulated second back, since
	 * the NTP subsystem has been notified via second_overflow. So
	 * instead we push xtime_nsec forward by the amount we underflowed,
	 * and add that amount into the error.
	 *
	 * We'll correct this error next time through this function, when
	 * xtime_nsec is not as small.
	 */
	if (unlikely((s64)clock->xtime_nsec < 0)) {
		s64 neg = -(s64)clock->xtime_nsec;
		clock->xtime_nsec = 0;
		clock->error += neg << (NTP_SCALE_SHIFT - clock->shift);
	}

	/* store full nanoseconds into xtime after rounding it up and
	 * add the remainder to the error difference.
	 */
	xtime.tv_nsec = ((s64)clock->xtime_nsec >> clock->shift) + 1;
	clock->xtime_nsec -= (s64)xtime.tv_nsec << clock->shift;
	clock->error += clock->xtime_nsec << (NTP_SCALE_SHIFT - clock->shift);

	/* check to see if there is a new clocksource to use */
	change_clocksource();
	update_vsyscall(&xtime, clock);
}

/**
 * getboottime - Return the real time of system boot.
 * @ts:		pointer to the timespec to be set
 *
 * Returns the time of day in a timespec.
 *
 * This is based on the wall_to_monotonic offset and the total suspend
 * time. Calls to settimeofday will affect the value returned (which
 * basically means that however wrong your real time clock is at boot time,
 * you get the right time here).
 */
void getboottime(struct timespec *ts)
{
	set_normalized_timespec(ts,
		- (wall_to_monotonic.tv_sec + total_sleep_time),
		- wall_to_monotonic.tv_nsec);
}

/**
 * monotonic_to_bootbased - Convert the monotonic time to boot based.
 * @ts:		pointer to the timespec to be converted
 */
void monotonic_to_bootbased(struct timespec *ts)
{
	ts->tv_sec += total_sleep_time;
}

unsigned long get_seconds(void)
{
	return xtime.tv_sec;
}
EXPORT_SYMBOL(get_seconds);

struct timespec current_kernel_time(void)
{
	struct timespec now;
	unsigned long seq;

	do {
		seq = read_atomic_seqbegin(&xtime_lock);
		now = xtime;
	} while (read_atomic_seqretry(&xtime_lock, seq));

	return now;
}
EXPORT_SYMBOL(current_kernel_time);

