"""
Therion Translation Checker
By: Marko Zakrajsek (marko@zakrajsek.org)
Last updated: 2026-05-27
===========================

Small utility script for checking missing translations
inside Therion language TXT files.

The script scans translation blocks formatted like:

    therion: title map
    en: Map
    de: Karte
    sl: Načrt

and reports all entries where the selected language
translation is missing.

Usage
-----
1. Place this script in the same folder as the TXT
   file (texts.txt), or adjust the file_path variable
   to point to the correct location.

2. Run the script from the command line:

       python check_lang_missing_strings.py <language>

3. Example:

       python check_lang_missing_strings.py sl
       python check_lang_missing_strings.py de
       python check_lang_missing_strings.py bg

Output
------
The script prints:
- total number of missing translations
- list of missing "therion:" keys

Statistics Mode
----------------
To get an overview of translation coverage for all languages, run:

    python check_lang_missing_strings.py stat

"""

from pathlib import Path
import sys

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

file_path = Path("texts.txt")
original_language = "en"

# -------------------------------------------------------------------
# Command-line arguments
# -------------------------------------------------------------------

if len(sys.argv) < 2:
    print("Usage:")
    print("    python check_lang_missing_strings.py <language>")
    print("    python check_lang_missing_strings.py stat")
    print("\nExample:")
    print("    python check_lang_missing_strings.py sl")
    print("    python check_lang_missing_strings.py stat")
    sys.exit(1)

command = sys.argv[1]

# -------------------------------------------------------------------
# Read translation file
# -------------------------------------------------------------------

text = file_path.read_text(encoding="utf-8")
blocks = text.split("\n\n")

# -------------------------------------------------------------------
# Statistics mode
# -------------------------------------------------------------------

if command == "stat":
    language_counts = {}

    for block in blocks:
        lines = block.strip().splitlines()

        if not lines:
            continue

        for line in lines:
            if ":" not in line:
                continue

            lang_code = line.split(":", 1)[0].strip()

            # Skip Therion keys
            if lang_code == "therion":
                continue

            language_counts[lang_code] = language_counts.get(lang_code, 0) + 1

    original_count = language_counts.get(original_language, 0)

    print(f'\nOriginal "{original_language}" strings: {original_count}\n')
    print("Translation statistics:\n")

    sorted_languages = sorted(
        language_counts.items(), key=lambda item: item[1], reverse=True
    )

    for lang_code, count in sorted_languages:

        if original_count > 0:
            percentage = (count / original_count) * 100
        else:
            percentage = 0

        print(f"{lang_code}: {count} strings ({percentage:.2f}%)")

    sys.exit(0)

# -------------------------------------------------------------------
# Missing translation mode
# -------------------------------------------------------------------

language = command
missing_translations = []

for block in blocks:
    lines = block.strip().splitlines()

    if not lines:
        continue

    key = None
    has_translation = False

    for line in lines:
        if line.startswith("therion:"):
            key = line.strip()
        elif line.startswith(f"{language}:"):
            has_translation = True

    if key and not has_translation:
        missing_translations.append(key)

print(f'\nMissing "{language}" translations: {len(missing_translations)}\n')

for item in missing_translations:
    print(item)
