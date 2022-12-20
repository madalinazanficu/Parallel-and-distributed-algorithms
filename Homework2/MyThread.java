import java.nio.file.*;
import java.util.ArrayList;
import java.util.concurrent.Semaphore;
import java.util.stream.*;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

// The current thread is responsible for landing tasks for orderds
public class MyThread extends Thread {

    ArrayList<String> orderdsIds = new ArrayList<>();
    ArrayList<Integer> quantities = new ArrayList<>();
    ArrayList<Semaphore> semaphores = new ArrayList<>();
    Integer start;
    Integer end;
    int id;
    String file;
    ExecutorService pool;
    FileWriter writerOrders;
    FileWriter writerProducts;
    String orderProducts;

    public MyThread(Integer start, Integer end, int id, String file, 
                    ExecutorService pool, FileWriter writerOrders, 
                    FileWriter writerProducts, String orderProducts) {
        this.start = start;
        this.end = end;
        this.id = id;
        this.file = file;
        this.pool = pool;
        this.writerOrders = writerOrders;
        this.writerProducts = writerProducts;
        this.orderProducts = orderProducts;
    }

    /* Extract the commands assigned to the current thread
        Read only the lines that are assigned to this thread
        Read from orderProducts file and searching products 
        tasks to the thread pool */
    @Override
    public void run() {
        Integer startByte = this.start;
        Integer endByte = this.end;

        try (RandomAccessFile srcFile =  new RandomAccessFile(this.file, "r")) {
            // Adjust the start byte to start on a new line
            if (startByte != 0) {
                srcFile.seek(startByte - 1);
                while (srcFile.read() != '\n') {
                    startByte++;
                }
            }

            // Adjust the end byte at the end of a line
            srcFile.seek(endByte);
            if (srcFile.read() != '\n') {
                while (srcFile.read() != '\n') {
                    endByte++;
                }
                endByte++;
            }

            // Read the commands
            srcFile.seek(startByte);
            while (srcFile.getFilePointer() < endByte) {
                String command = srcFile.readLine();
                String[] commadParts = command.split(",");
                    
                String orderId = commadParts[0];
                Integer quantity = Integer.valueOf(commadParts[1]);
                Semaphore semaphore = new Semaphore(-(quantity - 1));

                orderdsIds.add(orderId);
                quantities.add(quantity);
                semaphores.add(semaphore);
            }

            for (int i = 0; i < orderdsIds.size(); i++) {
                if (quantities.get(i) > 0) {
                    MyTask task = new MyTask(orderdsIds.get(i), semaphores.get(i), 
                                            0, pool, writerProducts, orderProducts);
                    pool.submit(task);
                    semaphores.get(i).acquire();
                    CommandCompleted(orderdsIds.get(i), quantities.get(i));
                }
            }
            
            srcFile.close();
        } catch(IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void CommandCompleted(String orderId, Integer quantity) {
        synchronized(writerOrders) {
            try {
                writerOrders.write(orderId + "," + quantity +  "," + "shipped" + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    
}
    

