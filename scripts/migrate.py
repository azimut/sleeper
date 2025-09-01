# GPT ftw!

import sqlite3
from datetime import datetime

# Path to your SQLite database
DB_PATH = 'sleeper.db'  # change this to your actual file

# Connect to the SQLite database
conn = sqlite3.connect(DB_PATH)
cursor = conn.cursor()

# Query all rows in the events table
cursor.execute("SELECT id, sleep_at, wakeup_at FROM events WHERE id <= 3539")
rows = cursor.fetchall()

# Define the input and output datetime formats
input_format = "%a %b %d %H:%M:%S %Y"
output_format = "%Y-%m-%d %H:%M:%S"

# Loop through rows and update with formatted datetime strings
for row in rows:
    row_id, sleep_at_str, wakeup_at_str = row

    try:
        sleep_at_dt = datetime.strptime(sleep_at_str, input_format)
        wakeup_at_dt = datetime.strptime(wakeup_at_str, input_format)

        new_sleep_at = sleep_at_dt.strftime(output_format)
        new_wakeup_at = wakeup_at_dt.strftime(output_format)

        # Update the row with the new formatted strings
        cursor.execute("""
            UPDATE events
            SET sleep_at = ?, wakeup_at = ?
            WHERE id = ?
        """, (new_sleep_at, new_wakeup_at, row_id))

    except ValueError as e:
        print(f"Skipping row {row_id} due to parsing error: {e}")

# Commit the changes and close the connection
conn.commit()
conn.close()

print("Datetime fields updated successfully.")
