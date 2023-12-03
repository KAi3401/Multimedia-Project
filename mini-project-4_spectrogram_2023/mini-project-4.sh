#!/bin/bash

mkdir -p result
temp_dir="tmp"
mkdir -p "$temp_dir"

# Check if the temp directory is not empty
if [ "$(ls -A $temp_dir)" ]; then
    echo "Cleaning up existing files in $temp_dir"
    rm -r "$temp_dir"/*
fi

echo "compiling sinegen.c"
gcc sinegen.c -o 1 -lm
echo "done"

echo -n > scp.txt #create scp.txt

echo "running sinegen.c"

BITS_PER_SAMPLE="16"
frequency=("0" "31.25" "500" "2000" "4000" "44" "220" "440" "1760" "3960")
amplitude=("100" "2000" "1000" "500" "250" "100" "2000" "1000" "500" "250")
fs_list=("8000" "16000")
wavetype=("sine" "sawtooth" "square" "triangle")
duration="0.1"
index=1
for fs in "${fs_list[@]}"; do
    for wave in "${wavetype[@]}"; do
        for i in {0..9}; do
            out_file="${fs}Hz${index}.wav"
            ./1 "$fs" "$BITS_PER_SAMPLE" "$wave" "${frequency[i]}" "${amplitude[i]}" "$duration" "$out_file"
            echo "$out_file" >> scp.txt
            ((index++))
        done
    done
done


echo "done"

sleep 1

echo "compiling cascade.c"
gcc cascade.c -o 2 -lm
echo "done"

echo "running cascade.c scp.txt s-8kHz.wav s-16kHz.wav"
./2 scp.txt s-8kHz.wav s-16kHz.wav
echo "done"

mv "scp.txt" "$temp_dir/"

echo "removing .exe file"
rm 1 2
echo "done"

echo "compiling spectrogram.c"
gcc spectrogram.c -o spectrogram -lm
echo "done"

echo "calculating spectrogram data and applying settings..."
echo ""
# Settings
settings=("1" "2" "3" "4")
window_sizes=("32" "32" "30" "30")
window_types=("Rectangular" "Hamming" "Rectangular" "Hamming")
dft_sizes=("32" "32" "32" "32")
frame_intervals=("10" "10" "10" "10")


# Create temp directory if it doesn't exist


# Files
input_files=("s-8kHz.wav" "s-16kHz.wav" "aeueo-8kHz.wav" "aeueo-16kHz.wav")

# Loop through settings and files
for ((i = 0; i < ${#settings[@]}; i++)); do
    setting=${settings[$i]}
    w_size=${window_sizes[$i]}
    w_type=${window_types[$i]}
    dft_size=${dft_sizes[$i]}
    f_itv=${frame_intervals[$i]}

    for input_file in "${input_files[@]}"; do
        # Generate output file name based on settings and input file
        output_file="${input_file%.*}_setting${setting}.txt"

        echo "applying setting $setting to $input_file. Please wait..."
        echo ""

        # Execute the C program with the specified settings
        ./spectrogram "$w_size" "$w_type" "$dft_size" "$f_itv" "$input_file" "$output_file"

        echo "Saving image result..."
        echo ""
        python3 spectshow.py "$output_file" "$input_file" "result/${output_file%.*}.pdf"

        echo "Done."
        echo ""
        echo "Setting $setting applied to $input_file. Output saved to $result_dir/$output_file"
        echo " $((++k)) / 16 "
        echo ""

        # Move the output file to the temp directory
        mv "$output_file" "$temp_dir/"
    done
done

echo "removing spectrogram.exe"
rm spectrogram

echo "All done"
