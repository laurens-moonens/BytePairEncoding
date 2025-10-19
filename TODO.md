Use cases:

[X] encode a file:
    -e input_file.txt

[X] decode a file:
    -d encoded_file.txt

[X] generate new text from an encoded file:
    -g encoded_file.txt <nr_of_tokens_to_generate>

[X] split bpe table and tokens

[X] more flexible flags:
    -e = --encode
    - ability to combine flags without having to specify multiple input or output files
    - better error handling and logging in case of missing flags

[ ] abstract away flags:
    [ ] options without subcommand
    [X] add description to flags
    [X] add "missing option" error
    [X] add "unknown command" error
    [X] order options based on when they were added in code, not alphabetically

- support for specifying input character size (unicode) and output token size
