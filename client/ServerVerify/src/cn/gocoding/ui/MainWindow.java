package cn.gocoding.ui;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tcp.TcpClient;
import cn.gocoding.common.tcp.TcpClientDelegate;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.server.protocol.Protocol;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * Created by liuke on 16/3/21.
 */
public class MainWindow {
    private JTextField ip_text;
    private JTextField port_text;
    private JButton connectButton;
    private JTextField id_text;
    private JTextField version_text;
    private JTextField serverType_text;
    private JTextField dataType_text;
    private JTextArea data_text;
    private JTextArea result_text;
    private JButton send_btn;
    private JButton disconnect_btn;
    private JPanel mainPanel;

    public static void main(String[] args) {
        JFrame frame = new JFrame("MainWindow");
        frame.setContentPane(new MainWindow().mainPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public MainWindow() {

        client.setDelegate(delegate);

        disconnect_btn.setEnabled(false);

        connectButton.addActionListener((ActionEvent e) -> {
            client.connect(ip_text.getText(), Integer.parseInt(port_text.getText()));
        });
        send_btn.addActionListener((ActionEvent e) -> {

            int clientID = Integer.parseInt(id_text.getText());
            byte version = Byte.parseByte(version_text.getText());
            byte serverType = Byte.parseByte(serverType_text.getText());
            byte dataType = Byte.parseByte(dataType_text.getText());

            String data = data_text.getText();

            Tuple2<ErrorCode, byte[]> p = Protocol.createProtocol(clientID, version, serverType, dataType, data.length(), data.getBytes());
            client.write(p._2().get());

        });
        disconnect_btn.addActionListener((ActionEvent e) -> {
            client.disconnect();
        });
    }

    private TcpClient client = new TcpClient();
    private TcpClientDelegate delegate = new TcpClientDelegate() {
        @Override
        public void connect() {
            result_text.append("connected\n");
            disconnect_btn.setEnabled(true);
            connectButton.setEnabled(false);
        }

        @Override
        public void read(byte[] data) {
            result_text.append("read data : ");
            result_text.append(data.toString());
            result_text.append("\n");
        }

        @Override
        public void write(byte[] data) {
            result_text.append("write data : ");
            result_text.append(data.toString());
            result_text.append("\n");
        }

        @Override
        public void close() {
            result_text.append("close connect\n");
            disconnect_btn.setEnabled(false);
            connectButton.setEnabled(true);
        }
    };
}
