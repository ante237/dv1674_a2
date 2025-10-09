size=$1
sum_time=0
sum_user=0
sum_sys=0
sum_util=0

for i in $(seq 1 5); do
    output=$(/usr/bin/time -v ./pearson_par data/${size}.data data_o/${size}_par 2>&1)

    time_us=$(echo "$output" | grep "Time elapsed" | awk '{print $3}' | tr -d 'µs')
    sum_time=$(echo "$sum_time + $time_us" | bc)

    usr_time=$(echo "$output" | grep "User time (seconds):" | awk '{print $4}')
    sum_user=$(echo "$sum_user + $usr_time" | bc)

    sys_time=$(echo "$output" | grep "System time (seconds):" | awk '{print $4}')
    sum_sys=$(echo "$sum_sys + $sys_time" | bc)

    cpu_util=$(echo "$output" | grep "Percent of CPU this job got:" | awk '{gsub("%","",$7); print $7}')
    sum_util=$(echo "$sum_util + $cpu_util" | bc)
done

mean_time=$(echo "scale=2; $sum_time / 5" | bc)
mean_ms=$(echo "scale=2; $mean_time / 1000" | bc)
mean_usr_time=$(echo "scale=2; $sum_user / 5" | bc)
mean_sys_time=$(echo "scale=2; $sum_sys / 5" | bc)
mean_cpu=$(echo "scale=2; $sum_util / 5" | bc)

echo "Mean time (total): $mean_ms ms ($mean_time µs)"
echo "Mean user time: $(echo "$mean_usr_time*1000" | bc) ms ($mean_usr_time s)"
echo "Mean system time: $(echo "$mean_sys_time*1000" | bc) ms ($mean_sys_time s)"
echo "CPU util: $mean_cpu %"
