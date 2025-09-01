select strftime('%Y-%m',sleep_at),
       cast(strftime('%H',sleep_at) as int),
       count(1)
  from events
 where etype == 'startup'
   -- and sleep_at like '%2025%'
 group by strftime('%Y-%m',sleep_at), strftime('%H', sleep_at);
