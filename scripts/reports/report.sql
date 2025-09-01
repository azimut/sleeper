select sleep_at, strftime('%H',sleep_at)
  from events
 where etype == 'startup';
--   and sleep_at like '%2025%';
