import java.nio.file.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Semaphore;
import java.util.stream.*;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;


public class MyTask implements Runnable {
    String order;
    Semaphore semaphore;
    Integer allBytes;
    ExecutorService pool;
    FileWriter writerProducts;
    String orderProducts;

    /* Used for sub-tasks */
    public MyTask(String order, Semaphore semaphore, Integer fileIndex, 
                ExecutorService pool, FileWriter writerProducts, 
                String orderProducts) {
        this.order = order;
        this.semaphore = semaphore;
        this.allBytes = fileIndex;
        this.pool = pool;
        this.writerProducts = writerProducts;
        this.orderProducts = orderProducts;
    }
    

    @Override
    public void run() {
        try  {
            BufferedReader reader = new BufferedReader(new FileReader(orderProducts));
            reader.skip(allBytes);
            String productLine = reader.readLine();
            if (productLine == null) {
                reader.close();
                return;
            }

            while (productLine != null) {
                Integer bytesCount = productLine.getBytes().length;
                allBytes += bytesCount + 1;

                String orderId = productLine.split(",")[0];
                String productId = productLine.split(",")[1];

                if (orderId.equals(order)) {
                    reader.close();
                    // The current product can be shipped
                    ProductShipped(orderId, productId);
    
                    // Release the parent task
                    semaphore.release();

                    MyTask task = new MyTask(order, semaphore, allBytes, 
                                            pool, writerProducts, orderProducts);
                    pool.submit(task);

                    break;
                } else {
                    productLine = reader.readLine();
                }
            }
        } catch (IOException e1) {
            e1.printStackTrace();
        }
    }

    public void ProductShipped(String orderId, String productId) {
        synchronized(writerProducts) {
            try {
                writerProducts.write(orderId + "," + 
                                    productId +  "," + 
                                    "shipped" + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}