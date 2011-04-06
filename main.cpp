#include <iostream>
#include <fstream>
#include <cstdlib>

#include "theora/theoradec.h"
#include "ogg/ogg.h"

using namespace std;

int main(int argc, char** argv)
{

    const int BUFFER_SIZE = 4096 * 1;
    th_info info;
    th_comment comment;
    th_setup_info *setup = NULL;

    char *pBuffer;

    ogg_page page;

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

    th_info_init(&info);
    th_comment_init(&comment);

    ogg_packet packet;


    bool headerRead = false;
    int i=0;
    bool streamInitialized = false;
    ogg_stream_state stream;

    while(!headerRead)
    {
        while((i = ogg_sync_pageout(&state, &page)) != 1)
        {
            cout << "reading file...\n";
            if(!file.good())
            {
                exit(1);
            }
            pBuffer = ogg_sync_buffer(&state, BUFFER_SIZE);


            file.read(pBuffer, BUFFER_SIZE);
            int bytesRead = file.gcount();

            if(file.fail() || file.eof())
            {
                cerr << "error reading file:" << file.rdstate() << "\n";
                exit(1);
            }

            if(ogg_sync_wrote(&state, bytesRead) == -1)
            {
                cerr << "error ogg_sync_wrote\n";
                exit(1);
            }
        }


        if(!streamInitialized && ogg_page_bos(&page))
        {
            ogg_stream_init(&stream, ogg_page_serialno(&page));
            cout << "new stream\n";
            streamInitialized = true;
        }

        if(ogg_stream_pagein(&stream, &page)!=0)
        {
            cerr << "ogg_stream_pagein error\n";
        }


        int p = ogg_stream_packetout(&stream, &packet);
        cout << "packet:" << p << "\n";


        int dec = th_decode_headerin(&info, &comment, &setup, &packet);
        cout << "dec:" << dec << "\n";

        if(dec == 0)
        {
            headerRead = true;
        }

    }



    cout << "frame ratio: " << info.frame_width << 'X' << info.frame_height << "\n"
    << "pixel format: ";
    switch(info.pixel_fmt)
    {
        case TH_PF_420 : cout << "TH_PF_420"; break;
        case TH_PF_RSVD : cout << "TH_PF_RSVD"; break;
        case TH_PF_422 : cout << "TH_PF_422"; break;
        case TH_PF_444 : cout << "TH_PF_444";
    }
    cout << "\nquality: " << info.quality << "\nbitrate: " << info.target_bitrate << "\n";



    th_dec_ctx *pDecCtx;

    pDecCtx = th_decode_alloc(&info, setup);
    if(pDecCtx == NULL)
    {
        cerr << "error allocing dec\n";
    }


    return 1;

}


