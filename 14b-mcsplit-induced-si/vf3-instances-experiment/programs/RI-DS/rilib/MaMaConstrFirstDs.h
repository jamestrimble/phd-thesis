/*
 * MaMaConstrFirst.h
 *
 *  Created on: Aug 5, 2012
 *      Author: vbonnici
 */
/*
Copyright (c) 2014 by Rosalba Giugno

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef MAMACONSTRFIRSTDS_H_
#define MAMACONSTRFIRSTDS_H_

#include "Graph.h"
#include "sbitset.h"

namespace rilib{

class MaMaConstrFirstDs : public MatchingMachine{
	sbitset *domains;
	int *domains_size;

public:

	MaMaConstrFirstDs(Graph& query, sbitset *_domains, int *_domains_size)
		: MatchingMachine(query), domains(_domains), domains_size(_domains_size){

	}

	virtual void build(Graph& ssg){

#ifdef MDEBUG
	std::cout<<"init mama...\n";
#endif

		enum NodeFlag {NS_CORE, NS_CNEIGH, NS_UNV};
		NodeFlag* node_flags = new NodeFlag[nof_sn]; 						//indexed by node_id
		int** weights = new int*[nof_sn];									//indexed by node_id
		int* t_parent_node = (int*) calloc(nof_sn, sizeof(int));			//indexed by node_id
		MAMA_PARENTTYPE* t_parent_type = new MAMA_PARENTTYPE[nof_sn];		//indexed by node id

		int  nof_single_domains = 0;

		for(int i=0; i<nof_sn; i++){
			node_flags[i] = NS_UNV;
			weights[i] = new int[3];
			weights[i][0] = 0;
			weights[i][1] = 0;
			//weights[i][2] = ssg.out_adj_sizes[i] + ssg.in_adj_sizes[i];
			weights[i][2] = ssg.out_adj_sizes[i] + ssg.in_adj_sizes[i];
			t_parent_node[i] = -1;
			t_parent_type[i] = PARENTTYPE_NULL;

			if(domains_size[i] == 1)
				nof_single_domains++;
		}




		int si = 0;
		int n;
		int nIT; int ni;
		int nnIT; int nni;
		int nqueueL = 0, nqueueR = 0;
		int maxi, maxv;
		int tmp;


#ifdef MDEBUG
	std::cout<<"single domains ["<<nof_single_domains<<"]...\n";
#endif

		if(nof_single_domains != 0){

			nqueueR = nof_single_domains;
			//nqueueR = 0;

			for(int n=0; n<nof_sn; n++){
				if(domains_size[n] == 1){

#ifdef MDEBUG
	std::cout<<"nqueueR("<<nqueueR<<") node["<<n<<"] si["<<si<<"]\n";
#endif

					map_state_to_node[si] = n;
					map_node_to_state[n] = si;
					t_parent_type[n] = PARENTTYPE_NULL;
					t_parent_node[n] = -1;

					//move queue left limit
					//nqueueL++;
					//update nodes' flags & weights
					node_flags[n] = NS_CORE;
					nIT = 0;
					while(nIT < ssg.out_adj_sizes[n]){
						ni = ssg.out_adj_list[n][nIT];
						if(ni != n && domains_size[ni]>1){
							weights[ni][0]++;
							weights[ni][1]--;

							if(node_flags[ni] == NS_UNV){
								node_flags[ni] = NS_CNEIGH;
								t_parent_node[ni] = n;
//								if(nIT < ssg.out_adj_sizes[n])
									t_parent_type[ni] = PARENTTYPE_OUT;
//								else
//									t_parent_type[ni] = PARENTTYPE_IN;
								//add to queue
								map_state_to_node[nqueueR] = ni;
								map_node_to_state[ni] = nqueueR;
								nqueueR++;

								nnIT = 0;
								while(nnIT < ssg.out_adj_sizes[ni]){
									nni = ssg.out_adj_list[ni][nnIT];
									weights[nni][1]++;
									nnIT++;
								}
							}
						}
						nIT++;
					}

					nIT = 0;
					while(nIT < ssg.in_adj_sizes[n]){
						ni = ssg.in_adj_list[n][nIT];
						if(ni != n  && domains_size[ni]>1){
							weights[ni][0]++;
							weights[ni][1]--;

							if(node_flags[ni] == NS_UNV){
								node_flags[ni] = NS_CNEIGH;
								t_parent_node[ni] = n;
//								if(nIT < ssg.out_adj_sizes[n])
//									t_parent_type[ni] = PARENTTYPE_OUT;
//								else
									t_parent_type[ni] = PARENTTYPE_IN;
								//add to queue
								map_state_to_node[nqueueR] = ni;
								map_node_to_state[ni] = nqueueR;
								nqueueR++;

								nnIT = 0;
								while(nnIT < ssg.in_adj_sizes[ni]){
									nni = ssg.in_adj_list[ni][nnIT];
									weights[nni][1]++;
									nnIT++;
								}
							}
						}
						nIT++;
					}

					si++;
				}
			}
		}
		nqueueL = nof_single_domains;


		/*for(int i=0; i<nof_single_domains; i++){
			std::cout<<i<<" "<<map_state_to_node[i]<<" "<<map_node_to_state[map_state_to_node[i]]<<"\n";
		}*/


#ifdef MDEBUG
	std::cout<<"others...\n";
#endif
		//if(nof_single_domains != nof_sn){
		while(si < nof_sn){

			if(nqueueL == nqueueR){
				//if queue is empty....
				maxi = -1;
				maxv = -1;
				nIT = 0;
				while(nIT < nof_sn){
					if(node_flags[nIT]==NS_UNV &&  weights[nIT][2] > maxv){
						maxv = weights[nIT][2];
						maxi = nIT;
					}
					nIT++;
				}
				map_state_to_node[si] = maxi;
				map_node_to_state[maxi] = si;
				t_parent_type[maxi] = PARENTTYPE_NULL;
				t_parent_node[maxi] = -1;

				nqueueR++;

				n = maxi;
				nIT = 0;
				while(nIT < ssg.out_adj_sizes[n]){
					ni = ssg.out_adj_list[n][nIT];
					if(ni != n){
						weights[ni][1]++;
					}
					nIT++;
				}
				while(nIT < ssg.in_adj_sizes[n]){
					ni = ssg.in_adj_list[n][nIT];
					if(ni != n){
						weights[ni][1]++;
					}
					nIT++;
				}
			}


			if(nqueueL != nqueueR-1){
				maxi = nqueueL;
				for(int mi=maxi+1; mi<nqueueR; mi++){
					if(wcompare(map_state_to_node[mi], map_state_to_node[maxi], weights) < 0){
						maxi = mi;
					}
				}
				tmp = map_state_to_node[nqueueL];
				map_state_to_node[nqueueL] = map_state_to_node[maxi];
				map_state_to_node[maxi] = tmp;
			}

			n = map_state_to_node[si];
			map_node_to_state[n] = si;

			//move queue left limit
			nqueueL++;
			//update nodes' flags & weights
			node_flags[n] = NS_CORE;
			nIT = 0;
			while(nIT < ssg.out_adj_sizes[n]){
				ni = ssg.out_adj_list[n][nIT];
				if(ni != n){
					weights[ni][0]++;
					weights[ni][1]--;

					if(node_flags[ni] == NS_UNV){
						node_flags[ni] = NS_CNEIGH;
						t_parent_node[ni] = n;
//						if(nIT < ssg.out_adj_sizes[n])
							t_parent_type[ni] = PARENTTYPE_OUT;
//						else
//							t_parent_type[ni] = PARENTTYPE_IN;
						//add to queue
						map_state_to_node[nqueueR] = ni;
						map_node_to_state[ni] = nqueueR;
						nqueueR++;

						nnIT = 0;
						while(nnIT < ssg.out_adj_sizes[ni]){
							nni = ssg.out_adj_list[ni][nnIT];
							weights[nni][1]++;
							nnIT++;
						}
					}
				}
				nIT++;
			}

			nIT = 0;
			while(nIT < ssg.in_adj_sizes[n]){
				ni = ssg.in_adj_list[n][nIT];
				if(ni != n){
					weights[ni][0]++;
					weights[ni][1]--;

					if(node_flags[ni] == NS_UNV){
						node_flags[ni] = NS_CNEIGH;
						t_parent_node[ni] = n;
//						if(nIT < ssg.out_adj_sizes[n])
//							t_parent_type[ni] = PARENTTYPE_OUT;
//						else
							t_parent_type[ni] = PARENTTYPE_IN;
						//add to queue
						map_state_to_node[nqueueR] = ni;
						map_node_to_state[ni] = nqueueR;
						nqueueR++;

						nnIT = 0;
						while(nnIT < ssg.in_adj_sizes[ni]){
							nni = ssg.in_adj_list[ni][nnIT];
							weights[nni][1]++;
							nnIT++;
						}
					}
				}
				nIT++;
			}

			si++;
		}
		//}


#ifdef MDEBUG
	std::cout<<"mama structs...\n";
#endif
		int e_count,o_e_count,i_e_count; int i;
		for(si = 0; si<nof_sn; si++){

#ifdef MDEBUG
	std::cout<<"\tnode ("<<si<<","<<map_state_to_node[si]<<") /"<<nof_sn<<"\n";
#endif

			n = map_state_to_node[si];

			//nodes_attrs[si] = ssg.nodes_attrs[n];
#ifdef MDEBUG
	std::cout<<"\tparents...\n";
#endif
			if(t_parent_node[n] != -1)
				parent_state[si] = map_node_to_state[t_parent_node[n]];
			else
				parent_state[si] = -1;
			parent_type[si] = t_parent_type[n];


#ifdef MDEBUG
	std::cout<<"\tedge counts...\n";
#endif
			e_count = 0;
			o_e_count = 0;
			for(i=0; i<ssg.out_adj_sizes[n]; i++){
				if(map_node_to_state[ssg.out_adj_list[n][i]] < si){
					e_count++;
					o_e_count++;
				}
			}
			i_e_count = 0;
			for(i=0; i<ssg.in_adj_sizes[n]; i++){
				if(map_node_to_state[ssg.in_adj_list[n][i]] < si){
					e_count++;
					i_e_count++;
				}
			}


			edges_sizes[si] = e_count;
			o_edges_sizes[si] = o_e_count;
			i_edges_sizes[si] = i_e_count;


#ifdef MDEBUG
	std::cout<<"\tmama edges ["<<edges_sizes[si]<<"]["<<o_edges_sizes[si]<<"]["<<i_edges_sizes[si]<<"]...\n";
#endif

			edges[si] = new MaMaEdge[e_count];

			e_count = 0;
			for(i=0; i<ssg.out_adj_sizes[n];i++){
#ifdef MDEBUG
	std::cout<<"\t\tneigh("<<ssg.out_adj_list[n][i]<<")\n";
#endif
				if(map_node_to_state[ssg.out_adj_list[n][i]] < si){

#ifdef MDEBUG
	std::cout<<"\t\tadd("<<map_node_to_state[ssg.out_adj_list[n][i]]<<","<<ssg.out_adj_list[n][i]<<")\n";
#endif
					edges[si][e_count].source = map_node_to_state[n];
					edges[si][e_count].target = map_node_to_state[ssg.out_adj_list[n][i]];

					edges[si][e_count].attr = ssg.out_adj_attrs[n][i];

					e_count++;
				}
			}
			for(i=0; i<ssg.in_adj_sizes[n];i++){
				if(map_node_to_state[ssg.in_adj_list[n][i]] < si){
#ifdef MDEBUG
	std::cout<<"\t\tadd("<<map_node_to_state[ssg.in_adj_list[n][i]]<<","<<ssg.in_adj_list[n][i]<<")\n";
#endif
					edges[si][e_count].target = map_node_to_state[n];
					edges[si][e_count].source = map_node_to_state[ssg.in_adj_list[n][i]];

					int x = ssg.in_adj_list[n][i];
					int y = n;
					for(int xi=0; xi<ssg.out_adj_sizes[x]; xi++){
						if(ssg.out_adj_list[x][xi] == y){
							edges[si][e_count].attr = ssg.out_adj_attrs[x][xi];
							break;
						}
					}

					e_count++;
				}
			}

#ifdef MDEBUG
	std::cout<<"\tdone\n";
#endif
		}

#ifdef MDEBUG
	std::cout<<"mama done\n";
#endif

		delete[] node_flags;
		for(int i=0; i<nof_sn; i++)
			delete[] weights[i];
		delete[] weights;
		free(t_parent_node);
		delete[] t_parent_type;

	}





private:

	int wcompare(int i, int j, int** weights){
		for(int w=0; w<3; w++){
			if(weights[i][w] != weights[j][w]){
				return weights[j][w] - weights[i][w];
			}
		}
		return i-j;
	}
};

}


#endif /* MAMACONSTRFIRST_H_ */