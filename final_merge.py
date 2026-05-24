#!/usr/bin/env python3
import sys

def is_dialogue(line):
    stripped = line.strip()
    return stripped.startswith('“') or stripped.startswith('"') or stripped.startswith('‘') or stripped.startswith("'")

def is_special(line):
    stripped = line.strip()
    return stripped.startswith('#') or stripped.startswith('---') or stripped.startswith('**') or stripped == ''

def merge_conservative(text):
    lines = [line.rstrip('\n') for line in text.split('\n')]
    output = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        if stripped == '':
            output.append('')
            i += 1
            continue
        if is_special(line):
            output.append(line)
            i += 1
            continue
        if is_dialogue(line):
            output.append(line)
            i += 1
            continue
        # Non-dialogue, non-special line
        # Start accumulating short lines until we hit a dialogue, special, or long line
        group = [stripped]
        j = i + 1
        while j < len(lines):
            next_line = lines[j]
            next_stripped = next_line.strip()
            if next_stripped == '':
                # Empty line breaks group
                break
            if is_special(next_line) or is_dialogue(next_line):
                break
            # If next line is long (>=40 chars), maybe start new paragraph
            if len(next_stripped) >= 40:
                # Long line could be start of new paragraph, but we can still include it in group?
                # Let's break to keep long lines as separate paragraphs.
                break
            group.append(next_stripped)
            j += 1
        # Merge group into one paragraph
        merged = ' '.join(group)
        output.append(merged)
        i = j
    
    # Ensure at most one empty line between paragraphs
    result = []
    prev_empty = False
    for line in output:
        if line == '':
            if not prev_empty:
                result.append('')
                prev_empty = True
        else:
            result.append(line)
            prev_empty = False
    
    return '\n'.join(result)

def main():
    if len(sys.argv) != 2:
        print("Usage: python final_merge.py <input_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    merged = merge_conservative(content)
    print(merged, end='')

if __name__ == '__main__':
    main()