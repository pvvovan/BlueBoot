package com.exameliza.jpgstr;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {

    private static final int SERVER_PORT = 8035;
    private static final String SERVER_IP = "192.168.0.94";

    private ImageView imageView;
    Handler updateConversationHandler;
    TextView txtView_state;
    Button btn_left;
    Button btn_right;
    Button btn_up;
    Button btn_down;
    SeekBar seekBar_speed;
    int speed = 0;
    boolean up_pressed = false;
    boolean down_pressed = false;
    boolean right_pressed = false;
    boolean left_pressed = false;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = (ImageView)findViewById(R.id.imageView);
        updateConversationHandler = new Handler();
        txtView_state = (TextView)findViewById(R.id.textView_state);
        btn_left = (Button)findViewById(R.id.btnLeft);
        btn_down = (Button)findViewById(R.id.btnDown);
        btn_up = (Button)findViewById(R.id.btnUp);
        btn_right = (Button)findViewById(R.id.btnRight);
        seekBar_speed = (SeekBar)findViewById(R.id.seekBarSpeed);

        seekBar_speed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                speed = progress;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        CameraThread camThread = new CameraThread();
        new Thread(camThread).start();
        CmdThread cmdThread = new CmdThread();
        new Thread(cmdThread).start();

        btn_left.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (MotionEvent.ACTION_DOWN == event.getAction()) {
                    txtView_state.setText("left " + String.valueOf(speed));
                    left_pressed = true;
                }
                else if (MotionEvent.ACTION_UP == event.getAction()) {
                    txtView_state.setText("");
                    left_pressed = false;
                }
                return true;
            }
        });
        btn_right.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (MotionEvent.ACTION_DOWN == event.getAction()) {
                    txtView_state.setText("right " + String.valueOf(speed));
                    right_pressed = true;
                }
                else if (MotionEvent.ACTION_UP == event.getAction()) {
                    txtView_state.setText("");
                    right_pressed = false;
                }
                return true;
            }
        });
        btn_down.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (MotionEvent.ACTION_DOWN == event.getAction()) {
                    txtView_state.setText("down " + String.valueOf(speed));
                    down_pressed = true;
                }
                else if (MotionEvent.ACTION_UP == event.getAction()) {
                    txtView_state.setText("");
                    down_pressed = false;
                }
                return true;
            }
        });
        btn_up.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (MotionEvent.ACTION_DOWN == event.getAction()) {
                    txtView_state.setText("up " + String.valueOf(speed));
                    up_pressed = true;
                }
                else if (MotionEvent.ACTION_UP == event.getAction()) {
                    txtView_state.setText("");
                    up_pressed = false;
                }
                return true;
            }
        });
    }

    class CmdThread implements Runnable {
        public CmdThread() { }
        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                send_command();
                SystemClock.sleep(100);
            }
        }
    }

    class CameraThread implements Runnable {
        private Socket clientSocket;
        private DataInputStream input;//private BufferedReader input;

        public CameraThread() { }

        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
                    this.clientSocket  = new Socket(serverAddr, SERVER_PORT);
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

                SystemClock.sleep(15);
            }
        }
    }

    void send_command()
    {
        try {
            DatagramSocket client_socket = new DatagramSocket(SERVER_PORT);
            InetAddress IPAddress =  InetAddress.getByName(SERVER_IP);
            byte[] data = new byte[1];
            data[0] = (byte) (speed << 4);
            DatagramPacket send_packet = new DatagramPacket(data, 1, IPAddress, SERVER_PORT);
            client_socket.send(send_packet);
            client_socket.close();
        }
        catch (IOException ignored) { }
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
