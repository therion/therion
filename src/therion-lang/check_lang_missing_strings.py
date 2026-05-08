"""
Therion Translation Checker
By: Marko Zakrajsek
Last updated: 2026-05-08
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
    file (texts.txt). Or adjust the file_path variable
    to point to the correct location.
2. Update the values of:
       file_path (if needed)
       language
3. Run:
       python check_lang_missing_strings.py


Output
------
The script prints:
- total number of missing translations
- list of missing "therion:" keys

Optional Improvements
---------------------
Possible future upgrades:
- auto-generate empty translation rows
- direct translating through this script
- compare multiple languages
"""

from pathlib import Path

# -------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------

# Translation file to check
file_path = Path("texts.txt")

# Language code to validate
language = "sl"

# -------------------------------------------------------------------
# Read translation file
# -------------------------------------------------------------------

# Read the entire file using UTF-8 encoding
text = file_path.read_text(encoding="utf-8")

# Translation entries are separated by empty lines
blocks = text.split("\n\n")

# List of missing translation keys
missing_translations = []

# -------------------------------------------------------------------
# Process translation blocks
# -------------------------------------------------------------------

for block in blocks:

    # Split block into individual lines
    lines = block.strip().splitlines()

    # Skip empty blocks
    if not lines:
        continue

    key = None
    has_translation = False

    # Check all lines inside the block
    for line in lines:

        # Main translation key
        if line.startswith("therion:"):
            key = line.strip()

        # Translation for selected language
        elif line.startswith(f"{language}:"):
            has_translation = True

    # Store missing translation keys
    if key and not has_translation:
        missing_translations.append(key)

# -------------------------------------------------------------------
# Output results
# -------------------------------------------------------------------

print(f'Missing "{language}" translations: {len(missing_translations)}\n')

for item in missing_translations:
    print(item)
