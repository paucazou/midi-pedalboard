#g++ -pedantic -Wall -I /usr/include/rtmidi/ -lasound -lpthread -lrtmidi -D__LINUX_ALSA__ encoder.cpp -o encoder

build_mock () {
    compiler=$1
    if [[ ! $compiler ]]; then
        compiler=g++
    fi
    $compiler -pedantic -Wall -Wextra -std=c++17 -I ./mock encoder.cpp main.cpp ./mock/wiringPi.cpp ./mock/RtMidi.cpp -o encoder -D DEBUG -D LOOP_NB=false -D MOCK
}

build_debug () {
    g++ -pedantic -Wall -Wextra -std=c++17\
        -D DEBUG\
        -I /usr/include/rtmidi/ \
        -I /usr/include/\
        -lasound -lpthread -lrtmidi -lwiringpi\
        -D__LINUX_ALSA__\
        encoder.cpp main.cpp\
        -o encoder
}
        

build_release () {
    g++ -std=c++17\
        -I /usr/include/rtmidi/ \
        -I /usr/include/\
        -lasound -lpthread -lrtmidi -lwiringpi\
        -D__LINUX_ALSA__\
        encoder.cpp main.cpp\
        -o encoder
    }
