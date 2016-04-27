#include "graph.h"
#include "sample.h"

using namespace std;

int V = 20000000; // the maximum number of nodes of the network

int find_or_newNode(map<int, int> &paperid_map, int paperid, Graph* graph)
{
    int id;
    if (paperid_map.find(paperid) == paperid_map.end())
    {
        id = newNode(graph, 'P');
        paperid_map.insert(pair<int, int>(paperid, id));
    }
    else id = paperid_map.find(paperid)->second;
    return id;
}

void initGraph(Graph* graph, map<string, int> &author_map, map<int, int> &paperid_map, map<string, int> &venue_map)
{
    int id = -1, i = -1, j = -1;
    FILE* file = fopen("/home/lux5/data/dblp/DBLP_Citation_2014_May/publications_small.txt", "r");
    if (file == NULL) { printf("ERROR: read txt file failed.\n"); exit(1); };
    char line[10000], abstract[10000];
    char title[2000], authors[2000], venue[2000]; 
    int year, paperid; 
    set<int> referenceid;

    while ( fgets(line, sizeof(line), file) )
    {
        //trim
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\r' || line[len-1] == '\n' || line[len-1] == '.')) --len; 
	line[len] = '\0';
        if (line[0] == '#')
        {
            switch ( line[1] )
            {
                case '*' : //printf("Title:%s", line);
		           strcpy(title, line + 2); break;
                case '@' : //printf("Authors:%s", line);
		           strcpy(authors, line + 2); break;
                case '%' : //printf("Reference:%s", line);
		           if (strlen(line)>3)
		               referenceid.insert( atoi(line + 2) );          break;
                case 'c' : //printf("Venue:%s", line);
		           strcpy(venue, line + 2); break;
                case 'i' : //printf("Index:%s", line);
		           paperid = atoi(line + 6); break;
                case 't' : //printf("Year:%s", line);
		           year = atoi(line + 2); break;
                case '!' : //printf("Abstract%s\n", line);
		           strcpy(abstract, line + 2); 
		           //paper
			   id = find_or_newNode(paperid_map, paperid, graph);
                           graph->item[id] = (Item *) new Paper(title, abstract, year);
			   //graph->item[id]->myprint();
			   set<int>::iterator set_it; 
			   //reference
			   for (set_it = referenceid.begin();set_it != referenceid.end();++set_it)
			   {
			       int tmp64 = find_or_newNode(paperid_map, *set_it, graph);
			       addEdge(graph, tmp64, id); //old paper points to new papers
			   }
			   referenceid.clear();
			   //authors
			   j = 0; // first char of the first author name
			   int tmp71 = strlen(authors);
                           for (i = 0; i <= tmp71; ++i )
                           {
                               if (authors[i] == ',' || authors[i] == '\0')
                               { 
                                   authors[i] = '\0';

				   string str((char *)(authors+j));
				   int a = -1;  
			           if (author_map.find(str) == author_map.end())
				   {
				       a = newNode(graph, 'A');
				       author_map.insert(pair<string, int>(str, a));
			               graph->item[a] = (Item *) new Author((char *)(authors + j));
				   }
				   else a = author_map.find(str)->second;
				   addEdge(graph, a, id); // connect author with his paper
			           //(graph->item[a])->myprint();
				   j = i+1;
                               }
                           };
			   //venue
                  	   int vi = -1;  
		           string strv((char *)venue);
			   if (venue_map.find(strv) == venue_map.end())
			   {
			       vi = newNode(graph, 'V');
			       venue_map.insert(pair<string, int>(strv, vi));
			       graph->item[vi] = (Item *) new Venue((char *)(venue));
			   }
			   else vi = venue_map.find(strv)->second;
			   addEdge(graph, id, vi); // connect paper with its venue 
			   //fflush(NULL);
            }
        } 
        //fgets(line, sizeof(line), file) )
    }
    fclose(file);
}

void removeGraph(Graph* graph)
{
    // remove all the out-going edges of a node 
    for (int i = 0; i < graph->cap; ++i)
       removeAllEdges(graph, i);
    // release the memory allocated to the graph
    deleteGraph(graph);
}

void printStatistic(Graph* graph)
{
    int sumPaper = 0, sumCitations = 0;
    for (int i = 0;i < graph->cap;++i)
    {
        if (graph->type[i] == 'P' and graph->item[i] != NULL) ++sumPaper;
	else continue;
        struct AdjListNode* pCrawl = graph->array[i].head;
	while (pCrawl)
	{
	    if (graph->type[pCrawl->dest] == 'P' && pCrawl->type == 'i')
	        ++sumCitations;
	    pCrawl = pCrawl->next;
	}
    }
    cout<<"#paper = "<< sumPaper <<", #citations "<< sumCitations <<endl;
}

void getSubset(set<int> &subset_dst, set<int> &subset_src, char t, char iioo, Graph* g)
{
    subset_dst.clear();
    set<int>::iterator src_it; 
    for (src_it = subset_src.begin(); src_it != subset_src.end(); ++src_it)
    {
        struct AdjListNode* pCrawl = g->array[*src_it].head;
        while (pCrawl)
        {
            if (pCrawl->type == iioo && g->type[pCrawl->dest] == t)
                subset_dst.insert(pCrawl->dest);
            pCrawl = pCrawl->next;
        }
    }
   /* 
    set<int>::iterator set_it;
    for (set_it = subset_dst.begin(); set_it != subset_dst.end(); ++set_it)
    {
        if (g->item[*set_it])
            printf("%c %d %s\n", g->type[*set_it], *set_it, (g->item[*set_it])->name);
	else 
            printf("%c %d No record\n", g->type[*set_it], *set_it); 
    }*/
}

void getNeighbors(set<int> &subset_dst, int src, char * format, Graph* g)
{
    int i =0;
    set<int> tmp, oldtmp;
    oldtmp.insert(src);
    subset_dst.clear();
    for (i = 0;i<strlen(format);i += 2)
    {
        getSubset(tmp, oldtmp, format[i+1], format[i], g);
	oldtmp = tmp;  
	tmp = set<int>();
    }
    subset_dst.insert(oldtmp.begin(), oldtmp.end());
}

// Personalized PagRank
void PPR(int tom, Graph * g)
{
    int number_of_iterations = 100;
    int K = 10;
    double prob_back = 0.8;
    int subsetplan = 1;

    set<int> final_authorset;
    set<int> him;
    set<int> his_papers;
    him.insert(tom);
    /***********find subset of authors start*************/
    if (subsetplan == 1)
    {   // all the authors in the same conferences

        set<int> his_venues;
        set<int> his_co_papers;
        // his papers
        getSubset(his_papers, him, 'P', 'o', g); 
        // the venues 
        getSubset(his_venues, his_papers, 'V', 'o', g);
        // the papers in same venues 
        getSubset(his_co_papers, his_venues, 'P', 'i', g);
        // the authors who publish papers in the same venues 
        getSubset(final_authorset, his_co_papers, 'A', 'i', g);
    }
    else if(subsetplan == 2)
    {
        set<int> set2; 
	getNeighbors(final_authorset, tom, "oPoPiA", g); 
	getNeighbors(set2, tom, "oPiPiA", g); 
	getNeighbors(set2, tom, "oPiPiA", g); 
	final_authorset.insert(set2.begin(), set2.end());
    }
    /***********find subset of authors end*************/

    map<int, double> score;
    set<int> neighbors; // the neighbors is determined by the relationship
    set<int>::iterator ait;
    for (ait = final_authorset.begin();ait != final_authorset.end(); ++ait)
    {
        g->item[*ait]->hold[0] = 0.0;
        g->item[*ait]->hold[1] = 0.0;
        //g->item[*ait]->myprint();
    }
    g->item[tom]->hold[0] = 100.0;
    g->item[tom]->hold[1] = 0.0;
    cout << "Subset of authors : size =" << final_authorset.size() << endl;
    cout << "Run PPR for " << g->item[tom]->name <<"..." << endl; 

    for (int iter = 0;iter < number_of_iterations; ++iter)
    {
        /******one iteration of the Personalized PageRank**********/
        set<int>::iterator ait;
        for (ait = final_authorset.begin();ait != final_authorset.end(); ++ait)
	{
            neighbors.clear();
	    getNeighbors(neighbors, *ait, (char *)("oPiA"), g);
	    int csize = neighbors.size();
	    set<int>::iterator cit;
	    for (cit = neighbors.begin();cit != neighbors.end(); ++cit)
	    {
	        if ( final_authorset.find(*cit) != final_authorset.end() )
		{
	            g->item[*cit]->hold[1] +=
		        (g->item[*ait]->hold[0] * (1.0 - prob_back)) / csize;
		}
	    }
	    g->item[tom]->hold[1] += 
		        g->item[*ait]->hold[0] * prob_back;
	}
        for (ait = final_authorset.begin();ait != final_authorset.end(); ++ait)
	{
	    g->item[*ait]->hold[0] = g->item[*ait]->hold[1];
	    g->item[*ait]->hold[1] = 0;
	}
    }
    /*********top K algorithm start***************/
    priority_queue<pair<int,double>,vector<pair<int,double> >,CompareDist> pq;
    double pqmin = -1;
    for (ait = final_authorset.begin();ait != final_authorset.end(); ++ait)
    {
        //cout << g->item[*ait]->name << ":" << g->item[*ait]->hold[0] << "%" << endl; 
	if (! pq.empty())
            pqmin = (pq.top()).second; 

	if (g->item[*ait]->hold[0] > pqmin)
	{
            //cout << g->item[*ait]->name << ":" << g->item[*ait]->hold[0] << "%" << endl; 
            pq.push(pair<int, double>(*ait, g->item[*ait]->hold[0]));
	}
	while (pq.size() > K) pq.pop();
    }
    //output the names
    while (! pq.empty())
    {
        pair<int, double> p = pq.top();
        cout << g->item[p.first]->name << ":" << p.second << "%" << endl; 
	pq.pop();
    }
    /*********top K algorithm end***************/
}

// Driver program to test above functions
int main(int argc, char *argv[])
{
    srand(time(NULL));

    // we create a graph (memory allocation)
    cout << "memory allocation...." << endl;
    struct Graph* graph = createGraph(V);
    // initialize the graph by adding edges
    cout << "read txt file...." << endl;

    clock_t tStart_read = clock();
    map<string, int> author_map;
    map<string, int> venue_map;
    map<int, int> paperid_map;
    initGraph(graph, author_map, paperid_map, venue_map);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart_read)/CLOCKS_PER_SEC);
    // print the graph
    //printGraph(graph);
    cout << "statistic...." << endl;

    clock_t tStart = clock();
    printStatistic(graph);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    clock_t tStart_PPR = clock();
    int yeyeid = author_map.find("Boleslaw K. Szymanski")->second;
    PPR(yeyeid, graph);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart_PPR)/CLOCKS_PER_SEC);

    removeGraph(graph);
    return 0;
}
