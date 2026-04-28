ipcs -m | awk 'NR>3{print $2}' | xargs -I{} ipcrm -m {} 2>/dev/null
ipcs -s | awk 'NR>3{print $2}' | xargs -I{} ipcrm -s {} 2>/dev/null
ipcs -q | awk 'NR>3{print $2}' | xargs -I{} ipcrm -q {} 2>/dev/null