#include "bridge.h"
using namespace std;

const int bignum = 10000000;

Bridge::Bridge(st idd, vector<st> portss){
	id = idd;
	ports = portss;
	for(int i=0; i<ports.size(); i++) port_type[ports[i]] = "DP";
	root_belief = idd;
	root_dist = 0;
	parent_bridge = idd;
}

Message::Message(st rb, int d, st s, st l){
	root_belief = rb;
	root_dist = d;
	sender = s;
	lan_name = l;
}

Lan::Lan(st nm, vector<st> bs){
	bridges = bs;
	name = nm;
	root_dist = bignum;
	designated_bridge = "xxx";
}

bool Bridge::update(Message m,bool trace, int time){

	if(trace){
		cout<<time<<" r "<<this->id<<" ("<<m.root_belief<<", "<<m.root_dist<<", "<<m.sender<<")"<<endl;
	}

	bool yes = false; st rp; 
	for(st x : this->ports) if(port_type[x]=="RP") rp = x;
	if(m.root_belief < this->root_belief){
		this->root_belief = m.root_belief;
		this->root_dist = 1 + m.root_dist;
		this->parent_bridge = m.sender; 
		this->port_type[m.lan_name] = "RP";
		for(st x : this->ports) if(x!=m.lan_name) this->port_type[x] = "DP";
		yes = true;
	}
	else if(m.root_belief == this->root_belief &&
			m.root_dist +1 == this->root_dist &&
			m.sender < this->parent_bridge){
		this->parent_bridge = m.sender;
		this->port_type[m.lan_name] = "RP";
		for(st x : this->ports) if(x!=m.lan_name) this->port_type[x] = "DP";
		yes = true;
	}
	else if(m.root_belief == this->root_belief &&
			m.root_dist +1 == this->root_dist &&
			m.sender == this->parent_bridge &&
			m.lan_name < rp){
		this->port_type[m.lan_name] = "RP";
		for(st x : this->ports) if(x!=m.lan_name) this->port_type[x] = "DP";
		yes = true;
	}
	return yes;
}

vector<Message> Bridge::send_message(bool trace, int time){
	vector<Message> vm;
	for(st x : this->ports){
		if(this->port_type[x]=="DP"){
			if(trace){
				cout<<time<<" s "<<this->id<<" ("<<this->root_belief<<", "<<this->root_dist<<", "<<this->id<<")"<<endl;
			}
			Message m(this->root_belief, this->root_dist, this->id,x);
			vm.pb(m);
		}
	}
	return vm;
}

bool Lan::update(Message m){
	if(m.root_dist < this->root_dist){
		this->root_dist = m.root_dist;
		this->designated_bridge = m.sender;
	}
}

void Bridge::print_ftable(){
	cout<<this->id<<":"<<endl;
	cout<<"HOST ID | FORWARDING PORT"<<endl;
	for(auto it = ftable.begin();it!=ftable.end();it++){
		cout<<it->first<<" | "<<it->second<<endl;
	}
}

vector<Packet> Bridge::send_packet(Packet p){
	//cout<<"send packet called "<<endl;
	//p.print();
	st l = p.lan_name;
	vector<Packet> vp;
	this->ftable[p.sender_host] = l;
	if(this->ftable.find(p.dest_host)!=this->ftable.end()){
			if(l!=this->ftable[p.dest_host]){
			Packet tmp(p.sender_host,p.dest_host,this->ftable[p.dest_host],this->id);
			vp.pb(tmp);
			return vp;
		}
	}
	else{
		for(st x : this->ports){
			if(this->port_type[x]!="NP" && x!=l){
				Packet tmp(p.sender_host,p.dest_host,x,this->id);
				//tmp.print();
				vp.pb(tmp);
			}
		}
		return vp;
	}
	
}