#include<vector>
#include<iostream>
#include<map>
#include<string>
#define st string
#define pb push_back
#define rep(i,n) for(int i=0;i<n;i++)
#define vst vector<st>
using namespace std;

class Message{
public:
	st root_belief;
	int root_dist;
	st sender;
	st lan_name;

	Message(st rb, int d, st s, st l);
};


class Lan{
public:
	st name;
	vector<st> bridges; // neighbour bridges
	int root_dist;
	st designated_bridge;

	Lan(st n, vector<st> bs);

	bool update(Message m);
};


class Packet{
public:
	st sender_host;
	st dest_host;
	st lan_name;
	st sender_bridge;

	Packet(st s, st d, st l, st b){
		sender_host = s;
		dest_host = d;
		lan_name = l;
		sender_bridge = b;
	}

	void print(){
		cout<<"Packet "<<sender_host<<" to "<<dest_host;
		cout<<" is at lan "<<lan_name<<" and was sent by ";
		cout<<sender_bridge<<endl; 
	}

};

struct host_compare{
	bool operator()(const st &a, const st &b) const {
		int aa = stoi(a.substr(1,a.size()-1));
		int bb = stoi(b.substr(1,b.size()-1));
		return  aa < bb;
	}
};

class Bridge{
public:
	st id;
	vector<st> ports; // same as neighbour lans
	map<st,st> port_type;
	st root_belief;
	int root_dist;
	st parent_bridge;
	
	map<st,st,host_compare> ftable; // host to port

	Bridge(st idd, vector<st> portss);

	bool update(Message m, bool trace, int time);

	vector<Message> send_message(bool trace, int time);

	void print_ftable();

	vector<Packet> send_packet(Packet p);

};

