# BytePairEncoding

This cli tool allows text to be encoded, decoded and inspected using byte pair encoding.
After encoding some text, the tool can also generate some new text (often gibberish) based on the original text.

E.g. Here is some generated text, based on the Bee Movie script:<br>
"I will be able live are you? A lot to floll the honey production this this is this?"

-------------------------------------------------------------------------------

## How to build

This project uses CMake to build.

``` sh
git clone https://github.com/laurens-moonens/BytePairEncoding.git
cd BytePairEncoding

mkdir build
cd build
cmake ..
cmake --build .
```

Only tested on Arch Linux using Make.
More testing to follow.

### Requirements
- [Cmake](https://cmake.org/download/) (version 3.14 or higher)


## How to use

Run `./bpe -h` for a full list of commands and options.

#### Encoding a file

``` sh
./bpe encode -i ../data/bee_movie_script.txt -b ../data/bee_movie.bpe
```
Optionally the encoded tokens can be outputted as well.<br>
`-t ../data/bee_movie.tkn`<br>

#### Decoding an encoded file

``` sh
./bpe decode -b ../data/bee_movie.bpe -t ../data/bee_movie.tkn -o ../data/bee_movie_decoded.txt 
```

#### Inspecting the generated BPE table

``` sh
./bpe inspect -b ../data/bee_movie.bpe
```

#### Generating new text using the BPE table

``` sh
./bpe generate -b ../data/bee_movie.bpe
```
Optionally the generated text can be outputted to a file.<br>
`-o ../data/bee_movie_generated.txt`<br>
Optionally the number of generated tokens can be specified (defaults to 100).<br>
`-c 69`<br>
