#ifndef _ANYKA_TIME_H_
#define _ANYKA_TIME_H_

#include <time.h>
#include <sys/time.h>

enum time_unit
{
    TIME_UNIT_MS = 0x00,        //millisecond
    TIME_UNIT_S,                //second
    TIME_UNIT_M                 //minute
};

/* calculate run time tag */
enum run_time_calc
{
	CALC_RUN_TIME_START = 0x00,		//tag: start
	CALC_RUN_TIME_END,				//tag: end
};

/** DelayTime: delay certain time
 * @secs[IN]: seconds
 * @milli_secs[IN]: milli-seconds
 * return: none
 */
void sleep_time(const int secs, const int milli_secs);

/** get_passed_seconds: passed total seconds from 1970-01-01 00:00:00
 * @void
 * return: passed total seconds
 */
static inline time_t get_passed_seconds(void)
{
	return time((time_t *)NULL);
}

/** get_tm_time: transfer cur_secs to real world date & time
 * @void
 * return: date & time from struct tm
 */
static inline struct tm* get_tm_time(void)
{
	time_t cur_secs = get_passed_seconds();
	return localtime(&cur_secs);
}

/** day_start_seconds: get current day total start seconds
 * @void
 * return: total seconds from 1970-1-1 00:00:00 to current day time 00:00:00
 */
time_t day_start_seconds(void);

/** get_day_seconds: current day time from today's 00:00:00
 * @void
 * return: diff seconds from today's 00:00:00
 */
time_t get_day_seconds(void);

/** day_clock_to_seconds: transfer current day time clock to seconds
 * @clock[IN]: current day time clock
 * return: seconds after transferred
 * notes: clock format: 06:45:20
 */
time_t day_clock_to_seconds(const char *clock);

/** day_secs_to_total: transfer day seconds to passed total seconds 
 *		from 1970-01-01 00:00:00
 * @void
 * return: passed total seconds after transferred
 */
time_t day_secs_to_total(time_t day_secs);

/** add_show_time: add current message show time
 * return: none
 */
void add_show_time(void);

/** time_string_to_value: transfer time string to time value
 * @time_str[IN]: time string
 * @value[OUT]: time value after transfer OK
 * return: 0 success; otherwise -1
 * notes: string format: yyyyMMdd-HHmmss, ex: 20160406-100606
 */
int time_string_to_value(const char *time_str, struct tm *value);

/** time_value_to_string: transfer time value to time string
 * @value[IN]: time value
 * @time_str[OUT]: time string after transfer OK
 * return: 0 success; otherwise -1
 * notes: string format: yyyyMMdd-HHmmss, ex: 20160406-100606
 */
int time_value_to_string(const struct tm *value, char *time_str);

/** secs_to_readable_string: transfer total seconds to readable time string
 * @secs: passed total seconds from 1970-01-01 00:00:00
 * return: readable time string after transferred
 * notes: string format: yyyy-MM-dd HH:mm:ss, ex: 2016-04-06 10:06:06
 *      time_str min len 20 bytes.
 * IMPORTANT: use ONLY ONCE after call this function.
*/
char* secs_to_readable_string(time_t secs);

/** get_readable_time_string: get current localtime and transfer to 
 *      readable time string
 * @void
 * return: readable time string after transferred
 * notes: string format: yyyy-MM-dd HH:mm:ss, ex: 2016-04-06 10:06:06
 *      time_str min len 20 bytes.
 */
char* get_readable_time_string(void);

/** diff_ms_time 
 * @old_time: old time 
 * @cur_time: current time 
 * return: diff time, uint: ms
 */
long diff_ms_time(struct timeval old_time, struct timeval cur_time);

/** calc_run_time
 * @tag: calc tag
 * return: used time, uint: ms
 */
long calc_run_time(int tag);

/** show_timeval
 * @cur_time: current time of struct timeval
 * return: none
 * notes: show at format: 2016-06-06 12:00:00-256
 */
void show_timeval(struct timeval *cur_time);

/** check_over_time
 * @cur_time: current time
 * @old_time: old time
 * @cycle_value: time over cycle value
 * @time_unit: time unit
 * return: 1-time over; 0-not yet
 */
int check_over_time(struct timeval *cur_time,
					struct timeval *old_time,
					unsigned int cycle_value,
	                int time_unit);

#endif
