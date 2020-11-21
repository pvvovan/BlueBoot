package com.exameliza.jpgstr;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.widget.ImageView;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {

    private static final int SERVERPORT = 8035;
    private static final String SERVER_IP = "192.168.0.88";
    private ImageView imageView;
    Handler updateConversationHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = (ImageView)findViewById(R.id.imageView);
        updateConversationHandler = new Handler();

        CommunicationThread commThread = new CommunicationThread();
        new Thread(commThread).start();
    }

    class CommunicationThread implements Runnable {
        private Socket clientSocket;
        private DataInputStream input;//private BufferedReader input;

        public CommunicationThread() {
        }

        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
                    this.clientSocket  = new Socket(serverAddr, SERVERPORT);
                    InputStream in = this.clientSocket.getInputStream();
                    this.input = new DataInputStream(in);

                    int len = 0;
                    byte[] data;
                    int len1 = this.input.readByte();
                    int len2 = this.input.readByte();
                    int len3 = this.input.readByte();
                    int len4 = this.input.readByte();
                    len = len1 + len2 * 256 + len3 * 256 * 256;
                    data = new byte[len];
                    if (len > 0) {
                        this.input.readFully(data,0,data.length);
                    }
                    updateConversationHandler.post(new UpdateUIThread(data));
                } catch (IOException e) {
                    e.printStackTrace();
                }

                SystemClock.sleep(30);
            }
        }

    }

    class UpdateUIThread implements Runnable {
        private byte[] byteArray;

        public UpdateUIThread(byte[] array){
            this.byteArray=array;
        }

        @Override
        public void run() {
            Bitmap bitmap = BitmapFactory.decodeByteArray(byteArray , 0, byteArray .length);
            imageView.setImageBitmap(bitmap);
        }
    }

}
