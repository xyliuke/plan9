package base


const MESSAGE_TYPE_SERVER byte = 0
const MESSAGE_TYPE_CLIENT byte = 1

type Message interface {
	ReceiveMessage([]byte)
	ReceiveMessageID(int, []byte)
	GetMessageType() byte//返回message_type定义类型
}

type MessageManager struct {
	delegates map[int]Message
}

func NewMessageManager() *MessageManager {
	m := new(MessageManager)
	m.delegates = make(map[int]Message)
	return m
}

func (this *MessageManager) Add(id int, msg Message)  {
	this.delegates[id] = msg
}

func (this *MessageManager) Remove(id int)  {
	delete(this.delegates, id)
}

func (this *MessageManager) Send(data []byte)  {
	for id, m := range this.delegates {
		if m.GetMessageType() == MESSAGE_TYPE_SERVER {
			m.ReceiveMessageID(id, data)
		}
	}
}

func (this *MessageManager) SendID(id int, data []byte)  {
	m, ok := this.delegates[id]
	if ok {
		if m.GetMessageType() == MESSAGE_TYPE_CLIENT {
			m.ReceiveMessage(data)
		} else if m.GetMessageType() == MESSAGE_TYPE_SERVER {
			m.ReceiveMessageID(id, data)
		}
	}
}
