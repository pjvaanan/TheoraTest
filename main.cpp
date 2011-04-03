#include <iostream>
#include <fstream>
#include <cstdlib>

#include "theora/theoradec.h"
#include "ogg/ogg.h"

using namespace std;

int main(int argc, char** argv)
{

    const int BUFFER_SIZE = 8096 * 10;
    th_info info;
    th_comment comment;
    th_setup_info *setup = NULL;

    char *pBuffer;

    ogg_sync_state state;


    if(argc != 2)
    {
        cerr << "usage: " << argv[0] << " <filename>\n";
        exit(1);
    }
    ifstream file(argv[1], ios_base::in | ios_base::binary);
    if(!file)
    {
        cerr << "could not read file " << argv[1] << "\n";
        exit(1);
    }

    ogg_sync_init(&state);
    pBuffer = ogg_sync_buffer(&state, BUFFER_SIZE);

    file.read(pBuffer, BUFFER_SIZE);

    if(file.fail() || file.eof())
    {
        cerr << "error reading file\n";
        exit(1);
    }

    ogg_sync_wrote(&state, BUFFER_SIZE);

    ogg_page page;
    ogg_sync_pageout(&state, &page);


    ogg_stream_state stream;
    ogg_stream_init(&stream, ogg_page_serialno(&page));

    ogg_stream_pagein(&stream, &page);

    ogg_packet packet;

    ogg_stream_packetout(&stream, &packet);


    th_info_init(&info);
    th_comment_init(&comment);
    int ret = th_decode_headerin(&info, &comment, &setup, &packet);
    cout << "frame:" << info.frame_width << 'X' << info.frame_height << "\n";


    th_dec_ctx *pDecCtx;

    pDecCtx = th_decode_alloc(&info, setup);
    if(pDecCtx == NULL)
    {
        cerr << "error allocing dec\n";
    }


    return 1;

}


