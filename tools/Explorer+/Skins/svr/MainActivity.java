package com.example.slideshow;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;

import com.android.util.ImageUtil;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class MainActivity extends Activity {

	protected TextView text;
	protected RelativeLayout background;
	protected SocketHandler socketHandler;
	protected SocketThread socketThread;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
              
        setContentView(R.layout.activity_main);
        
        text = (TextView) findViewById(R.id.txt);        
        background = (RelativeLayout) findViewById(R.id.main);
        
        text.setText("aaaaaaaaaa\nddddddddddddd\naaaaaaaaaaaaaaaaaaaaaaaaa");
        String path = getSDPath() + "/image/DSC01855.jpg";
        new AsyncLoadImg().execute(path);
        
        socketHandler = new SocketHandler();
        socketThread = new SocketThread();
        socketThread.start();
    }

    public String getSDPath() {
    	File sdPath = null;
    	boolean exist = Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED);
    	if(exist)
    		sdPath = Environment.getExternalStorageDirectory();
    	
    	return sdPath.toString();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    public boolean onOptionsItemSelected(MenuItem item) {
    	Intent intent = new Intent();
    	intent.setClass(MainActivity.this, SettingActivity.class);
    	startActivity(intent);
    	
    	return true;
    }
    
    class AsyncLoadImg extends AsyncTask<String, BitmapDrawable, Void> {
    	
    	protected void onProgressUpdate(BitmapDrawable... bitmapDrawables) {
    		background.setBackgroundDrawable(bitmapDrawables[0]);
    	}
    	
    	protected Void doInBackground(String... strings) {
    		int width = getWindowManager().getDefaultDisplay().getWidth();
            int height = getWindowManager().getDefaultDisplay().getHeight();
            Bitmap bmp = ImageUtil.resize(strings[0], width, height, true);
            if(bmp == null)
            	return null;
            
            publishProgress(new BitmapDrawable(bmp));            
    		return null;
    	}
    }
    
    // 接收到消息
    class SocketHandler extends Handler {
    	public void handleMessage(Message msg) {
    	
    	}
    }
    
    class SocketThread extends Thread {
    	protected Socket socket;
    	DataInputStream istrm;
    	DataOutputStream ostrm;
    	
    	public void run() {
    		try {
				socket = new Socket("192.168.1.100", 8878);
				ostrm = new DataOutputStream(socket.getOutputStream());
				istrm = new DataInputStream(socket.getInputStream());
				
				packet pkt = new packet((byte)1, (byte)0, (byte)0, (byte)0, (char)4);
				pkt.data = new String("lili").getBytes();
				ostrm.write(pkt.buff());
				
				while(true) {
					if(!socket.isConnected()) {
						continue;
					}
					
					try {
						Thread.sleep(10);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
    }
    
    class packet
    {
    	public packet()
    	{
    		identify = (byte)0xFF;
    		id = 0;
    		cmd = 0;
    		flag = 0;
    		size = 0;
    		offset = 0;
    		len = 0;
    		Arrays.fill(hash, (byte)0);
    		data = null;
    	}
    	
    	public packet(byte cmd, byte flag, int size, int offset, char len)
    	{
    		this.identify = (byte)0xFF;
    		this.cmd = cmd;
    		this.flag = flag;
    		this.size = size;
    		this.offset = offset;
    		this.len = len;
    		Arrays.fill(hash, (byte)0);
    		this.data = null;
    	}

    	public byte[] buff()
    	{
    		byte[] buf;
    		int buf_len = 55;
    		if(len != 0)
    			buf_len += len;
    		
    		buf = new byte[buf_len];
    		buf[0] = identify;
    		buf[1] = (byte)id;
    		buf[2] = (byte)(id >> 8);
    		buf[3] = cmd;
    		buf[4] = flag;
    		buf[5] = (byte)size;
    		buf[6] = (byte)(size >> 8);
    		buf[7] = (byte)(size >> 16);
    		buf[8] = (byte)(size >> 24);
       		buf[9] = (byte)offset;
    		buf[10] = (byte)(offset >> 8);
    		buf[11] = (byte)(offset >> 16);
    		buf[12] = (byte)(offset >> 24);
    		buf[13] = (byte)len;
    		buf[14] = (byte)(len >> 8);    		
    		System.arraycopy(hash, 0, buf, 15, 40);
    		if(data != null)
    			System.arraycopy(data, 0, buf, 55, len);
    		
    		return buf;
    	}
    	
    	public byte identify = (byte)0xFF; // 0xFF
    	public char id;
    	public byte cmd;
    	public byte flag;
    	public int size;
    	public int offset;
    	public char len;
    	public byte[] hash = new byte[40];
    	public byte data[];
    };

}
