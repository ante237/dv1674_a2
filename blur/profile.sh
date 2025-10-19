RADIUS=15
THREADS=8
INPUT_DIR="data"
OUTPUT_DIR="data_o"
MASSIF_DIR="massif_out"
CALLGRIND_DIR="callgrind_out"

echo "--------------------------------------------------"
echo " Profiling ./blur_par on all images in $INPUT_DIR"
echo " Using radius=$RADIUS, threads=$THREADS"
echo "--------------------------------------------------"

# Loop through all .ppm images in INPUT_DIR
for img in "$INPUT_DIR"/*.ppm; do
    filename=$(basename "$img")
    name="${filename%.*}"  # remove extension
    outfile="$OUTPUT_DIR/blur_${name}_par.ppm"

    echo "[+] Running on $filename"

    # --- Run MASSIF ---
    valgrind --tool=massif \
        --massif-out-file="$MASSIF_DIR/massif_${name}.out" \
        ./blur_par "$RADIUS" "$img" "$outfile" "$THREADS"

    echo "    ↳ Massif done: $MASSIF_DIR/massif_${name}.out"

    # --- Run CALLGRIND ---
    # valgrind --tool=callgrind \
    #     --main-stacksize=67108864 \
    #     --max-stackframe=8388608 \
    #     --num-callers=20 \
    #     --smc-check=all \
    #     --callgrind-out-file="$CALLGRIND_DIR/callgrind_${name}.out" \
    #     ./blur_par "$RADIUS" "$img" "$outfile" "$THREADS"

    # echo "    ↳ Callgrind done: $CALLGRIND_DIR/callgrind_${name}.out"
done

echo "--------------------------------------------------"
echo " Profiling completed!"
echo " Massif outputs  : $MASSIF_DIR/"
echo " Callgrind outputs: $CALLGRIND_DIR/"
echo "--------------------------------------------------"