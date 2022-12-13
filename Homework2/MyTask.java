import java.nio.file.*;
import java.util.concurrent.Semaphore;
import java.util.stream.*;
import java.io.IOException;


public class MyTask implements Runnable {
    String order;
    Semaphore semaphore;
    Integer fileIndex;

    /* Used for sub-tasks */
    public MyTask(String order, Semaphore semaphore, Integer fileIndex) {
        this.order = order;
        this.semaphore = semaphore;
        this.fileIndex = fileIndex;
    }

    @Override
    public void run() {

        // End of file
        if (fileIndex >= Tema2.productsSize) {
            return;
        }
    
        try (Stream<String> productLines = Files.lines(Paths.get(Tema2.orderProducts))) {
            String productLine = productLines.skip(fileIndex).findFirst().get();

            String[] productParts = productLine.split(",");
            String orderId = productParts[0];
            String productId = productParts[1];

            if (orderId.equals(order)) {
                
                // The current product can be shipped
                ProductShipped(orderId, productId);

                // Release the parent task
                semaphore.release();
            }
            // Submit a new child task
            MyTask task = new MyTask(order, semaphore, fileIndex + 1);
            Tema2.pool.submit(task);
            
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void ProductShipped(String orderId, String productId) {
        synchronized(Tema2.writerProducts) {
            try {
                Tema2.writerProducts.write(orderId + "," + productId +  "," + "shipped" + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
