#!/bin/bash
# Usage: ./run_transform_folder.sh <input_dir> <output_dir> <perl_script>

in_dir="$1"
out_dir="$2"
script="$3"

if [ -z "$in_dir" ] || [ -z "$out_dir" ] || [ -z "$script" ]; then
  echo "Usage: $0 <input_dir> <output_dir> <perl_script>"
  exit 1
fi

if [ ! -d "$in_dir" ]; then
  echo "❌ Input directory '$in_dir' not found"
  exit 1
fi

if [ ! -f "$script" ]; then
  echo "❌ Perl script '$script' not found"
  exit 1
fi

mkdir -p "$out_dir"

shopt -s nullglob # avoids literal *.c if no match
for f in "$in_dir"/*.{c,h}; do
  [ -e "$f" ] || continue
  base=$(basename "$f")
  echo "🔧 Processing $base ..."
  perl "$script" "$f" "$out_dir/$base"
done

echo "✅ Done. Converted files saved in: $out_dir"
