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
    
    // ���յ���Ϣ
    class SocketHandler extends Handler {
    	public void handleMessage(Message msg) {
    	
    	}
    }
	
    class circle_queue {
		public circle_queue(int size) {
			resize(size);
		}
		
		public boolean full() {
			return full(1);
		}
		
		public boolean full(int count) {
			return length() + count >= size;
		}
		
		public boolean empty() {
			return m_data == null || m_size == 0 || m_front == m_rear;
		}
		
		public int length() {
			return (m_rear - m_front + m_size) % m_size;
		}
		
		public int size() {
			return m_size;
		}
		
		public int front() {
			return m_front;
		}
		
		public void reset() {
			m_front = 0;
			m_rear = 0;
		}
		
		public void resize(int size) {
			if(size <= 0)
				return;
			
			m_front = 0;
			m_rear = 0;
			m_size = size;
			m_data = new byte[size];
		}
		
		public int push(byte[] data) {
			return push(data, 0, data.length);
		}
		
		public int push(byte[] data, int offset, int len) {
			if(m_size == 0 || m_data == null)
				return 0;
	
			// �ж϶����Ƿ���,���˾�����.
			if(full(len))
				reset();

			// ����׷�����ݺ��Ƿ�ᳬ��ĩβ.
			int left = m_rear + len - m_size;

			// ������ĩβ,�������Ĳ���д���б�ͷ��.
			if(left > 0)
			{
				int count = m_size - m_rear;
				memcpy(m_data, m_rear, data, offset, count);
				memcpy(m_data, 0, data, offset + count, len - count);
			}
			// û�г���,ֱ��׷�ӵ����к���.
			else
			{
				memcpy(m_data, m_rear, data, offset, len);
			}

			m_rear = (m_rear + len) % m_size;

			return 0;
		}

		public byte[] pop(int size)
		{
			if(empty())
				return NULL;

			size = size > length() ? length() : size;

			byte[] data = new byte[size];
			int left = (m_front + size) - m_size;
			if(left > 0)
			{
				int count = m_size - m_front;
				memcpy(data, 0, m_data, m_front, count);
				memcpy(data, count, m_data, 0, size - count);
			}
			else
			{
				memcpy(data, 0, m_data, m_front, size);
			}

			m_front = (m_front + size) % m_size;

			return data;
		}

		public void skip(int size)
		{
			if(empty())
				return;
			
			size = size > length() ? length() : size;
			m_front = (m_front + size) % m_size;
		}

		public byte get(int index)
		{
			if(empty() || index < 0 || index >= m_size)
				return (byte)0;
			
			return m_data[index];
		}
		
		public int get(byte[] data)
		{
			if(empty())
				return 0;
			
			int size = data.length;
			size = size > length() ? length() : size;

			int left = (m_front + size) - m_size;
			if(left > 0)
			{
				int count = m_size - m_front;
				memcpy(data, 0, m_data, m_front, count);
				memcpy(data, count, m_data, 0, size - count);
			}
			else
			{
				memcpy(data, 0, m_data, m_front, size);
			}

			return size;
		}
		
		protected void memcpy(Object dst, int dstOffset, Object src, int srcOffset, int len) {
			System.arrayCopy(src, srcOffset, dst, dstOffset, len);
		}
		
		protected int m_front = 0;
		protected int m_rear = 0;
		protected int m_size = 0;
		protected byte m_data[] = null;
	}
	
	class cmd
	{
		static const int report_name = 1,
		static const int text = 2,
		static const int image = 3,
		static const int video = 4,
	}

	class flag
	{
		static const int ack = 1,
	};

	class client {
		circle_queue m_recv_queue = new circle_queue(16 * 1024);;
		
		public void handleRcv(byte data[]) {
			int len = data.length;
			
			packet pkt;
			int header_len = packet.header_len();
			if(!m_recv_queue.empty())
			{
				m_recv_queue.push(data);
				if(m_recv_queue.get(m_recv_queue.front()) != (byte)0xFF)
				{
					handle_close();
					return;
				}

				if(m_recv_queue.length() < (int)header_len)
					return;

				byte pktbuf[] = new byte[header_len];
				m_recv_queue.get(pktbuf);
				pkt.set(pktbuf);
				if(m_recv_queue.length() < (int)header_len + pkt.len)
					return;

				m_recv_queue.skip(header_len);
				pkt.data = m_recv_queue.pop(pkt.len);
			}
			else if(len < header_len)
			{
				m_recv_queue.push(data, len);
				return;
			}
			else
			{
				pkt.set(data, header_len);
				if(len < header_len + pkt.len)
				{
					m_recv_queue.push(data);
					return;
				}
				else if(len > header_len + pkt.len)
				{
					m_recv_queue.push(data, header_len + pkt.len, len - header_len - pkt.len);
				}

				pkt.set_data(data, header_len, pkt.len);
			}
			
			switch(pkt.cmd)
			{
				case cmd.text:
					break;
					
				case cmd.image:
				case cmd.video:
					break;
			}
		}
	}
	
    class SocketThread extends Thread {
    	SocketChannel socket;
		Slector selector;
    	DataInputStream istrm;
    	DataOutputStream ostrm;
    	byte buff[] = new byte[8192];
		client clnt = new client();
		
    	public void run() {
    		try {
				socket = new Socket("192.168.1.100", 8878);
				socket.configureBlocking(false);
				selector = Selector.open();
				socket.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE);
				
				ostrm = new DataOutputStream(socket.getOutputStream());
				istrm = new DataInputStream(socket.getInputStream());
				
				packet pkt = new packet((byte)1, (byte)0, (byte)0, (byte)0, (char)4);
				pkt.data = new String("lili").getBytes();
				ostrm.write(pkt.buff());
				
				while(true) {
					if(socket.isClosed() || !socket.isConnected()) {
						continue;
					}
					
					selector.select(0);
					Set<SelectionKey> readKeys = selector.selectedKeys();
					Iterator<SelectionKey> iterator = readKeys.iterator();
					while (iterator.hasNext()) {
						SelectionKey key = (SelectionKey) iterator.next();
						iterator.remove();
						
						if (key.isReadable()) {
							int size = istrm.read(buff);
							clnt.handleRecv(buff, 0, size);
						} else if (key.isWritable()) {
							
						}
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
		public byte identify = (byte)0xFF; // 0xFF
    	public char id = 0;
    	public byte cmd =;
    	public byte flag = 0;
    	public int size = 0;
    	public int offset = 0;
    	public char len = 0;
    	public byte hash[] = new byte[40];
    	public byte data[] = null;
		private byte m_buff[] = null;
		
		public static int header_len()
		{
			return 55;
		}

    	public packet()
    	{
    		Arrays.fill(hash, (byte)0);
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
    	}

    	public byte[] buff()
		{
			return buff(true, false);
		}
		
		public byte[] buff(boolean fill)
		{
			return buff(fill, false);
		}
		
    	public byte[] buff(boolean fill, boolean refill)
    	{
    		if(fill && (refill || m_buff == null)
    		{
				int buff_len = header_len();
				if(len != 0)
					buff_len += len;
				
				m_buff = new byte[buff_len];
				m_buff[0] = identify;
				m_buff[1] = (byte)id;
				m_buff[2] = (byte)(id >> 8);
				m_buff[3] = cmd;
				m_buff[4] = flag;
				m_buff[5] = (byte)size;
				m_buff[6] = (byte)(size >> 8);
				m_buff[7] = (byte)(size >> 16);
				m_buff[8] = (byte)(size >> 24);
				m_buff[9] = (byte)offset;
				m_buff[10] = (byte)(offset >> 8);
				m_buff[11] = (byte)(offset >> 16);
				m_buff[12] = (byte)(offset >> 24);
				m_buff[13] = (byte)len;
				m_buff[14] = (byte)(len >> 8);    		
				System.arraycopy(hash, 0, m_buff, 15, 40);
				if(data != null)
					System.arraycopy(data, 0, m_buff, buff_len, len);
    		}
			
    		return m_buff;
    	}
		
		public void set_data(byte data[], int offset, int len)
		{
			this.data = new byte[len];
			System.arrayCopy(data, offset, this.data, 0, len);
		}
		
		public void set(byte data[], int size)
		{
			if(data.length < size || size != header_len())
				return;
			
			this.identify = data[0];
			this.id = data[1] + (int)data[2] << 8;
			this.cmd = data[3];
			this.flag = data[4];
			this.size = data[5] + int(data[6]) << 8 + int(data[7]) << 16 + int(data[8]) << 24;
			this.offset = data[9] + int(data[10]) << 8 + int(data[11]) << 16 + int(data[12]) << 24;
			this.len = data[13] + (int)data[14] << 8;
			System.arraycopy(data, 15, this.hash, 0, 40);
		}
    };
}
