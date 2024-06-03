#pragma comment(lib,"ws2_32.lib")
#include "RoboCatPCH.h"

int main()
{
	SocketUtil com;
	com.StaticInit();
	TCPSocketPtr ClientSocket = com.CreateTCPSocket(INET);
	uint32_t ipmemory;

	if (!InetPton(AF_INET, "127.0.0.1", &ipmemory))
	{
		com.ReportError("InetPton");
		return -1;
	}

	SocketAddressPtr sap = make_shared<SocketAddress>(htonl(ipmemory), 8000);
	int result = ClientSocket->Connect(*sap);
	if (result != NOERROR)
		return -1;


	ReplicationManager* rm=new ReplicationManager;
	LinkingContext* link = rm->GetLinkingContext();

	OBR.RegisterCreationFunction<RoboCat>();

	char* temporaryBuffer = (char*)malloc(kMaxPacketSIze);

	ReplicationAction ra = RA_None;
	uint32_t NetworkId = -1;
	uint32_t receiveByteCount=0;

	InputMemoryBitStream* stream = nullptr;
	GameObject* go = nullptr;

	rm->mRPCmanager->ReigisterUnwrapFunction('PSND', UnwrapPlaySound);

	while (1)
	{
		receiveByteCount = ClientSocket->Receive(temporaryBuffer, kMaxPacketSIze);
		cout << "\n\n##########���� �� ##########\n\n";

		if (!(receiveByteCount > 0))
		{
			ClientSocket->~TCPSocket();
			com.CleanUp();
			return 0;
		}
		stream = new InputMemoryBitStream(temporaryBuffer, receiveByteCount*8);
		while (stream->GetRemainingBitCount())
		{
			auto[ra,NetworkId]=rm->ProcessReplicationAction(*stream);
			switch (ra)
			{
			case RA_Create:
				cout << "��ü ���� NetworkId=" << NetworkId << endl;
				go = link->GetGameObject(NetworkId);
				cout << "health=" << go->GetHealth()<<endl;
				cout << "meowCount=" << go->GetMeowCount()<<endl;
				break;
            case RA_Update:
				cout << "��ü ������Ʈ NetworkId=" << NetworkId << endl;
				go = link->GetGameObject(NetworkId);
				cout << "health=" << go->GetHealth()<<endl;
				cout << "meowCount=" << go->GetMeowCount()<<endl;
				break;
			case RA_Destroy:
				cout << "��ü ���� NetworkId=" << NetworkId << endl;
				break;
			case RA_RPC:
				cout << "RPC Complete" << endl;
				break;
			}// ����ġ
		}//while(Remaining

	}
	
	getchar();

	//InputMemoryStream stream(temporaryBuffer, (uint32_t)receiveByteCount);

	//uint32_t packettype;
	//stream.Read(packettype);
	//cout << "packet type =" << packettype << endl;

	//rm->ReceiveWorld(stream);


	// ����� ���� ��ȯ
	//RoboCat* robo = dynamic_cast<RoboCat*>(go);

	//RoboCatPtr->Read(stream);

	/*for (auto& go : rm->GetObjectsReplicatedToMe())
	{
		cout << "ReceiveByteCount = " << receiveByteCount << endl;
	
		cout << "�ش� ��ü ���� �Ϸ�" << endl;
	}
		getchar();*/

		return 0;
}

