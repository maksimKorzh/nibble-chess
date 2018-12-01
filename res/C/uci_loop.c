while(1)  // UCI Loop
    {
        memset(&l[0], 0, sizeof(l));
        fflush(stdout);

        if(!fgets(l, 2400, stdin))
            continue;

        if(l[0] == '\n')
            continue;

        if (!strncmp(l, "ucinewgame", 10))
        {
            A("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
            P();
        }

        else if(!strncmp(l, "isready", 7))
        {
            printf("readyok\n");
            continue;
        }

        else if (!strncmp(l, "uci", 3))
        {
            printf("id name Tighty\n");
            printf("id author Maksim Korzh\n");
            printf("uciok\n");
        }

        else if(!strncmp(l, "position startpos moves", 23))
        {
	        A("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "); P(); char *m = l; m += 23;
	        while(*m) { if(*m == ' ') { *m++; V v = Y(S, E, m); M(S, v); S = 24 - S; E = v.K; } *m++; } P();
        }

        else if(!strncmp(l, "position startpos", 17))
        {
            A("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
            P();
        }

        else if(!strncmp(l, "position fen", 12))
        {
            char *f = l; f += 13; A(f); char *m = l;
            while(strncmp(m, "moves", 5)) { *m++; if(*m == '\0') break; } m += 4;
            if(*m == 's')
            {
                while(*m) { if(*m == ' ') { *m++; V v = Y(S, E, m); M(S, v); S = 24 - S; E = v.K; } *m++; }
            }
            P();
        }

        else if (!strncmp(l, "go depth", 8))
        {
            char *g = l; g += 9; int d = *g - '0';
            int s = X(S, E, -10000, 10000, d, 0, q);
            printf("info score cp %d \n", s);
            printf("bestmove "); O(q->m.f, q->m.t, q->m.o); printf("\n");
        }

        else if(!strncmp(l, "quit", 4))
            break;
    }
