#include "bridge.h"
#include<iostream>
#include<sstream>
#include<vector>
#include<map>
#include<string>
#include<set>
#include<algorithm>
using namespace std;

#define prv print_vector
#define prvi print_vector<int>
template <typename T>
void print_vector(vector<T> v){
	for(int i=0;i<v.size();i++) cout<<v[i]<<" ";
	cout<<endl;
}


vector<st> empty;

////////////////////////////////


vector<Lan> all_lans;
vector<Bridge> all_bridges;

// conectivity
map<st,vst> btol;
map<st,vst> ltob;
map<st,st> htol;



int get_lan(st nm){
	rep(i,all_lans.size()){
		if(all_lans[i].name == nm) return i;
	}
}

int get_bridge(st bid){
	rep(i,all_bridges.size()){
		if(all_bridges[i].id == bid) return i;
	}
}

bool same_root_belief(vector<Message> vm,st root_bid){
	bool yes = !(vm.size() == 0);
	for(Message m : vm){
		if(m.root_belief!=root_bid) yes = false;
	}
	return yes;
}

void process_to_receive(vector<Message> to_receive, bool trace, int time){
	for(Message m : to_receive){
		vst bs = ltob[m.lan_name];
		for(st bid : bs){
			if(bid != m.sender){
				all_bridges[get_bridge(bid)].update(m,trace,time);
			}
		}
	}
}

vector<Packet> process_packets(vector<Packet> vp){
	vector<Packet> out;
	for(Packet p : vp){
		//p.print();
		st lan_name = p.lan_name;
		//if(htol[p.dest_host] == lan_name) continue;
		st sender_bridge = p.sender_bridge;
		vst bs = ltob[lan_name];
		for(st bid : bs){
			if(bid != sender_bridge && all_bridges[get_bridge(bid)].port_type[lan_name]!="NP"){
				vector<Packet> tmp = all_bridges[get_bridge(bid)].send_packet(p);
				for(Packet p : tmp) out.pb(p);
			}
		}
	}
	return out;
}

void lan_process(vector<Message> vm){
	for(Message m : vm){
		all_lans[get_lan(m.lan_name)].update(m);
	}
}


int main(){
	bool trace;
	cin>>trace;

	set<st> lan_names;
	st word; st line; 
	int n; cin>>n;
	getline(cin,line); //empty line

	for(int i=0; i<n; i++){	
		getline(cin,line);
		stringstream X(line);
		vst words; 
		while(getline(X, word, ' ')) words.pb(word);

		vst lanlist(words.begin()+1,words.end()); // splicing 
		Bridge b(words[0].substr(0,words[0].size()-1), lanlist);
		all_bridges.pb(b); 
		btol[b.id]=lanlist;

		for(int i=1;i<words.size();i++) lan_names.insert(words[i]);
	
	}

	for(st x : lan_names){ Lan l(x,empty); all_lans.pb(l); } 

	for(auto x : all_bridges) for(st nm : x.ports){ all_lans[get_lan(nm)].bridges.pb(x.id); ltob[nm].pb(x.id); }


	// at this point ltob and btol are filled 
	// also all_lans and all_bridges are filled


	/*for(auto x = btol.begin();x!=btol.end();x++) prv<st>(x->second);
	cout<<"```"<<endl;
	for(auto x = ltob.begin();x!=ltob.end();x++){
		cout<<x->first<<":- ";
		prv<st>(x->second);
	}*/


	vector<Message> to_receive;
	vector<Message> to_send;
	int time = 0;
	st root_bid = all_bridges[0].id;


	while(!same_root_belief(to_receive,root_bid)){

		process_to_receive(to_receive,trace,time);

		for(Bridge b : all_bridges) for(Message m : b.send_message(trace,time)) to_send.pb(m);

		to_receive = to_send;
		to_send.clear(); 
		time++;
		
	}

	for(Bridge b : all_bridges) for(st x : b.ports) if(b.port_type[x] == "DP") all_bridges[get_bridge(b.id)].port_type[x] = "NP";

	lan_process(to_receive);
	for(Lan l : all_lans){
		st db = l.designated_bridge;
		all_bridges[get_bridge(db)].port_type[l.name] = "DP";
	}

	rep(i,all_bridges.size()){
		sort(all_bridges[i].ports.begin(),all_bridges[i].ports.end());
	}

	for(Bridge b : all_bridges){
		cout<<b.id<<":";
		for(st x : b.ports) cout<<" "<<x<<"-"<<b.port_type[x];
		cout<<endl;	
	}
	


	// part 2

	for(int i=0; i<all_lans.size(); i++){	
		getline(cin,line);
		stringstream X(line);
		vst words; 
		while(getline(X, word, ' ')) words.pb(word);

		vst hostlist(words.begin()+1,words.end()); // splicing
		for(st h : hostlist) htol[h] = words[0].substr(0,words[0].size()-1);

	}

	//for(auto it = htol.begin();it!=htol.end();it++) cout<<it->first<<" "<<it->second<<endl;

	int m; cin>>m;
	st senderhost, desthost;
	vector<Packet> to_process;
	rep(i,m){
		cin>>senderhost>>desthost;
		Packet ori(senderhost,desthost,htol[senderhost],"");
		to_process.pb(ori);
		time = 0;
		while(!to_process.empty() && time<10){
			to_process = process_packets(to_process);
			time++;
		}

		for(Bridge b : all_bridges) b.print_ftable();
		cout<<endl;
	}


}