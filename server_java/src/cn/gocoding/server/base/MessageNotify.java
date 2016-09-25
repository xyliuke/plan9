package cn.gocoding.server.base;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

/**
 * 消息通知机制
 * Created by liuke on 16/3/10.
 */
public class MessageNotify {

    public synchronized void send(int id, byte[] data) {

        List<WeakReference<MessageNotifyRecevier>> del = new ArrayList<>();
        for (WeakReference<MessageNotifyRecevier> recevier : receviers) {
            if (recevier.get() != null) {
                recevier.get().receiveMessage(id, data);
            } else {
                del.add(recevier);
            }
        }

        receviers.remove(del);
    }

    public synchronized void send(int id, int clientID, byte[] data) {
        List<WeakReference<MessageNotifyRecevier>> del = new ArrayList<>();
        for (WeakReference<MessageNotifyRecevier> recevier : receviers) {
            if (recevier.get() != null) {
                recevier.get().receiveMessage(id, clientID, data);
            } else {
                del.add(recevier);
            }
        }

        receviers.remove(del);
    }

    public synchronized void addRecevier(MessageNotifyRecevier recevier) {
        boolean exist = false;
        for (WeakReference<MessageNotifyRecevier> recv : receviers) {
            if (recv.get() != null && recv.get().equals(recevier)) {
                exist = true;
            }
        }
        if (!exist) {
            receviers.add(new WeakReference<>(recevier));
        }
    }

    public synchronized void removeRecevier(MessageNotifyRecevier recevier) {
        WeakReference<MessageNotifyRecevier> exist = null;
        for (WeakReference<MessageNotifyRecevier> recv : receviers) {
            if (recv.get() != null && recv.get().equals(recevier)) {
                exist = recv;
            }
        }
        if (exist != null) {
            receviers.remove(exist);
        }
    }

    private List<WeakReference<MessageNotifyRecevier>> receviers = new ArrayList<>();
}
