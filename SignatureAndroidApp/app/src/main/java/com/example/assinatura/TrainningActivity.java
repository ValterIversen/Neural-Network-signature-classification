package com.example.assinatura;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.concurrent.TimeUnit;

import android.os.Bundle;

public class TrainningActivity extends AppCompatActivity {

    final static String path = Environment.getExternalStorageDirectory().getPath() + "/UserSignature/TrainingData/";
    final static String TAG = TrainningActivity.class.getName();
    Button mClear, mGetSign, mCancel;
    TextView mText;
    File file;
    LinearLayout mContent;
    View view;
    signature mSignature;
    Bitmap bitmap;

    //Variables for the time weights
    long BegginTime, EndTime, startTime;
    ArrayList<Long> Times = new ArrayList<Long>();

    //Variables for the Trajectory Sampling
    float eventBegginX, eventBegginY;
    ArrayList<Float[]> TrajectoryXY = new ArrayList<Float[]>();

    int imgOriginalHeight, imgOriginalWidth, imgProcessedHeight, imgProcessedWidth;

    // Creating Separate Directory for saving Generated Images
    String DIRECTORY = Environment.getExternalStorageDirectory().getPath() + "/UserSignature/TrainingData/";
    String pic_name = "";
    String StoredPath = "";

    public static  String ReadFile( Context context, String fileName){
        String line = null;

        try {
            FileInputStream fileInputStream = new FileInputStream (new File(path + fileName));
            InputStreamReader inputStreamReader = new InputStreamReader(fileInputStream);
            BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
            StringBuilder stringBuilder = new StringBuilder();

            while ( (line = bufferedReader.readLine()) != null )
            {
                stringBuilder.append(line + System.getProperty("line.separator"));
            }
            fileInputStream.close();
            line = stringBuilder.toString();

            bufferedReader.close();
        }
        catch(FileNotFoundException ex) {
            Log.d(TAG, ex.getMessage());
        }
        catch(IOException ex) {
            Log.d(TAG, ex.getMessage());
        }
        return line;
    }

    public static boolean saveToFile(String data, String fileName, boolean Append){
        try {
            new File(path  ).mkdir();
            File file = new File(path + fileName);
            if (!file.exists()) {
                file.createNewFile();
            }
            FileOutputStream fileOutputStream = new FileOutputStream(file,Append);
            fileOutputStream.write((data).getBytes());

            return true;
        }  catch(FileNotFoundException ex) {
            Log.d(TAG, ex.getMessage());
        }  catch(IOException ex) {
            Log.d(TAG, ex.getMessage());
        }
        return  false;
    }

    static {
        System.loadLibrary("NeuralNetwork");
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_trainning);

        mContent = (LinearLayout) findViewById(R.id.canvasLayout);
        mText = (TextView) findViewById(R.id.tvTitle);
        mSignature = new signature(getApplicationContext(), null);
        mSignature.setBackgroundColor(Color.WHITE);
        // Dynamically generating Layout through java code
        mContent.addView(mSignature, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        mClear = (Button) findViewById(R.id.clear);
        mGetSign = (Button) findViewById(R.id.getsign);
        mGetSign.setEnabled(false);
        //mCancel = (Button) findViewById(R.id.cancel);
        view = mContent;
        mGetSign.setOnClickListener(onButtonClick);
        mClear.setOnClickListener(onButtonClick);
        //mCancel.setOnClickListener(onButtonClick);

        // Method to create Directory, if the Directory doesn't exists
        file = new File(DIRECTORY);
        if (!file.exists()) {
            file.mkdir();
        }

    }

    public void onResume()
    {
        super.onResume();

        String file = ReadFile(getApplicationContext(), "authorTraining.txt").replace("\n", "");
        if(file != null){
            String[] fileText = file.split(";");
            if(Integer.parseInt(fileText[1]) > 10) {
                Intent intent = new Intent(TrainningActivity.this, MainActivity.class);
                startActivity(intent);
            }
            else{
                pic_name = "signature-" + fileText[0].trim() + "-" + fileText[1];
                StoredPath = DIRECTORY + pic_name + ".png";
                if (Integer.parseInt(fileText[1]) < 11) {
                    mText.setText("Signature number: " + fileText[1]);
                }
            }
        }
        else{
            Intent intent = new Intent(TrainningActivity.this, MainActivity.class);
            startActivity(intent);
        }
    }

    Button.OnClickListener onButtonClick = new Button.OnClickListener() {
        @Override
        public void onClick(View v) {
            // TODO Auto-generated method stub
            if (v == mClear) {
                Log.v("log_tag", "Panel Cleared");
                recreate();
            } else if (v == mGetSign) {
                Log.v("log_tag", "Panel Saved");
                if(isStoragePermissionGranted()){
                    view.setDrawingCacheEnabled(true);
                    if(mSignature.save(view, StoredPath)) {
                        Toast.makeText(getApplicationContext(), "Successfully Saved", Toast.LENGTH_SHORT).show();
                        // Calling the same class
                        recreate();
                    }
                    else
                        Toast.makeText(getApplicationContext(), "An Error Has Occurred", Toast.LENGTH_SHORT).show();
                }
            } else if(v == mCancel){
                Log.v("log_tag", "Panel Canceled");
                Intent intent = new Intent(TrainningActivity.this, MainActivity.class);
                startActivity(intent);
            }
        }
    };


    public boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (getApplicationContext().checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
                return true;
            } else {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
                return false;
            }
        } else {
            return true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){

            view.setDrawingCacheEnabled(true);
            if(mSignature.save(view, StoredPath)) {
                Toast.makeText(getApplicationContext(), "Successfully Saved", Toast.LENGTH_SHORT).show();
                // Calling the same class
                recreate();
            }
            else
                Toast.makeText(getApplicationContext(), "An Error Has Occurred", Toast.LENGTH_SHORT).show();
        }
        else
        {
            Toast.makeText(this, "The app was not allowed to write to your storage. Hence, it cannot function properly. Please consider granting it this permission", Toast.LENGTH_LONG).show();
        }
    }

    public class signature extends View {

        private static final float STROKE_WIDTH = 5f;
        private static final float HALF_STROKE_WIDTH = STROKE_WIDTH / 2;
        private Paint paint = new Paint();
        private Path path = new Path();

        private float lastTouchX;
        private float lastTouchY;
        private final RectF dirtyRect = new RectF();

        public signature(Context context, AttributeSet attrs) {
            super(context, attrs);
            paint.setAntiAlias(true);
            paint.setColor(Color.BLACK);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeJoin(Paint.Join.ROUND);
            paint.setStrokeWidth(27f);
        }

        public boolean save(View v, String StoredPath) {
            EndTime = System.currentTimeMillis() - BegginTime;
            if (bitmap == null) {
                bitmap = Bitmap.createBitmap(mContent.getWidth(), mContent.getHeight(), Bitmap.Config.RGB_565);
            }
            Canvas canvas = new Canvas(bitmap);
            try {
                // Output the file
                FileOutputStream mFileOutStream = new FileOutputStream(StoredPath);
                v.draw(canvas);

                //Store the image resolution.
                double OriginalWidth = (double)bitmap.getWidth(),  OriginalHeight = (double)bitmap.getHeight();

                bitmap = Bitmap.createScaledBitmap(bitmap,(int)(bitmap.getWidth()*0.2), (int)(bitmap.getHeight()*0.2), true);
                double ProcessedWidth = (double)bitmap.getWidth(),  ProcessedHeight = (double)bitmap.getHeight();

                //Get the number of pixels
                double ImgPixelsQuantity = ProcessedHeight * ProcessedWidth;
                double[] blackPixelHistogram = GetBlackPixelHistogram(bitmap, ImgPixelsQuantity);

                //Store the img
                bitmap.compress(Bitmap.CompressFormat.PNG, 10, mFileOutStream);

                //The Trim remove the blank parts of the img
                bitmap = TrimBitmap(bitmap);
                //Get percentage of trimmed image with original
                double trimmedWidth =  (bitmap.getWidth() - 1.0) / (ProcessedWidth - 1.0);
                double trimmedHeigth = (bitmap.getHeight() - 1.0) / (ProcessedHeight - 1.0);

                String fileData = "in: " + trimmedWidth + " " + trimmedHeigth + " ";
                fileData += blackPixelHistogram[0] + " " + blackPixelHistogram[1] + " " + blackPixelHistogram[2] + " " + blackPixelHistogram[3] + " ";

                //The total time that starts at the first touch
                fileData += EndTime + " ";

                //Time during finger movements/path is stored
                long timesSum = 0;
                for(int i = 0; i < Times.size(); i++){
                    timesSum += Times.get(i);
                }

                fileData += timesSum + " ";

                //Fingers-up quantity
                fileData += Times.size() + " ";

                double trajectoryXIn = 0.0,trajectoryXOut = 0.0, trajectoryYIn = 0.0, trajectoryYOut = 0.0;
                // I run tought a variable in which I store the start and end points (in X and Y coordinates) and perform the sum already normalizing
                for (int i = 0; i < TrajectoryXY.size(); i++)
                {
                    //fileData += TrajectoryXY.get(i)[0] + "," + TrajectoryXY.get(i)[1] + "," + TrajectoryXY.get(i)[2] + "," + TrajectoryXY.get(i)[3] + ";";
                    trajectoryXIn += (double)TrajectoryXY.get(i)[0] / (double)OriginalWidth;
                    trajectoryYIn += (double)TrajectoryXY.get(i)[1] / (double)OriginalWidth;
                    trajectoryXOut += (double)TrajectoryXY.get(i)[2] / (double)OriginalWidth;
                    trajectoryYOut += (double)TrajectoryXY.get(i)[3] / (double)OriginalWidth;
                }

                fileData += trajectoryXIn + " " + trajectoryYIn + " " + trajectoryXOut + " " + trajectoryYOut + "\n" + "out: 0.0\n";

                //To limit the numbers of signatures collected, i store the number in a txt together with the author's name
                String file = ReadFile(getApplicationContext(), "authorTraining.txt").replace("\n", "");
                String[] fileText = file.split(";");
                saveToFile(fileText[0] + ";" + (Integer.parseInt(fileText[1]) + 1), "authorTraining.txt", false);

                //Save the file with the name. Like: signUpData_Valter.txt
                saveToFile(fileData, "trainingData_" + fileText[0].trim() + ".txt", true);

                //If we reach the maximum number of signatures, we finish the collect and write in the trainingDataSet.txt which store all the datasets for training
                if(Integer.parseInt(fileText[1]) == 10){
                    String TrainingDataSet = ReadFile(getContext(), "trainingData_" + fileText[0].trim() + ".txt");
                    saveToFile(TrainingDataSet, "trainingDataSet.txt", true);
                }
                return true;

            } catch (Exception e) {
                Log.v("log_tag", e.toString());
                return false;
            }

        }

        public void clear() {
            path.reset();
            invalidate();
            mGetSign.setEnabled(false);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            canvas.drawPath(path, paint);
        }


        @Override
        public boolean onTouchEvent(MotionEvent event) {
            float eventX = event.getX();
            float eventY = event.getY();
            mGetSign.setEnabled(true);

            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    if (BegginTime == 0){
                        BegginTime = System.currentTimeMillis();
                    }
                    startTime = System.currentTimeMillis();
                    eventBegginX = eventX;
                    eventBegginY = eventY;
                    break;
                case MotionEvent.ACTION_UP:
                    Times.add(System.currentTimeMillis() - startTime);
                    TrajectoryXY.add(new Float[]{eventBegginX, eventBegginY, eventX, eventY});
                    break;
            }
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    path.moveTo(eventX, eventY);
                    lastTouchX = eventX;
                    lastTouchY = eventY;
                    return true;

                case MotionEvent.ACTION_MOVE:

                case MotionEvent.ACTION_UP:
                    resetDirtyRect(eventX, eventY);
                    int historySize = event.getHistorySize();
                    for (int i = 0; i < historySize; i++) {
                        float historicalX = event.getHistoricalX(i);
                        float historicalY = event.getHistoricalY(i);
                        expandDirtyRect(historicalX, historicalY);
                        path.lineTo(historicalX, historicalY);
                    }
                    path.lineTo(eventX, eventY);
                    break;

                default:
                    debug("Ignored touch event: " + event.toString());
                    return false;
            }

            invalidate((int) (dirtyRect.left - HALF_STROKE_WIDTH),
                    (int) (dirtyRect.top - HALF_STROKE_WIDTH),
                    (int) (dirtyRect.right + HALF_STROKE_WIDTH),
                    (int) (dirtyRect.bottom + HALF_STROKE_WIDTH));

            lastTouchX = eventX;
            lastTouchY = eventY;

            return true;
        }

        private void debug(String string) {

            Log.v("log_tag", string);

        }

        private void expandDirtyRect(float historicalX, float historicalY) {
            if (historicalX < dirtyRect.left) {
                dirtyRect.left = historicalX;
            } else if (historicalX > dirtyRect.right) {
                dirtyRect.right = historicalX;
            }

            if (historicalY < dirtyRect.top) {
                dirtyRect.top = historicalY;
            } else if (historicalY > dirtyRect.bottom) {
                dirtyRect.bottom = historicalY;
            }
        }

        private void resetDirtyRect(float eventX, float eventY) {
            dirtyRect.left = Math.min(lastTouchX, eventX);
            dirtyRect.right = Math.max(lastTouchX, eventX);
            dirtyRect.top = Math.min(lastTouchY, eventY);
            dirtyRect.bottom = Math.max(lastTouchY, eventY);
        }

    }

    public static double[] GetBlackPixelHistogram(Bitmap bmp, double ImgPixelsQuantity){
        int x = bmp.getWidth() - 1;
        int y = bmp.getHeight() - 1;
        
        double[] blackPixelHistogram = new double[4];
        int area1 = 0, area2 = 0, area3 = 0, area4 = 0;
        for (int i = 0; i < y; i++){
            for (int j = 0; j < x / 4; j++){
                if (bmp.getPixel(j, i) != Color.WHITE)
                    area1++;
            }
            for (int j = x / 4; j < x / 2; j++){
                if (bmp.getPixel(j, i) != Color.WHITE)
                    area2++;
            }
            for (int j = x / 2; j < (x / 4 * 3); j++){
                if (bmp.getPixel(j, i) != Color.WHITE)
                    area3++;
            }
            for (int j = (x / 4 * 3); j < x; j++){
                if (bmp.getPixel(j, i) != Color.WHITE)
                    area4++;
            }
        }
        blackPixelHistogram[0] = area1 / (ImgPixelsQuantity / 4);
        blackPixelHistogram[1] = area2 / (ImgPixelsQuantity / 4);
        blackPixelHistogram[2] = area3 / (ImgPixelsQuantity / 4);
        blackPixelHistogram[3] = area4 / (ImgPixelsQuantity / 4);

        return blackPixelHistogram;
    }

    public static Bitmap TrimBitmap(Bitmap bmp) {
        try {
            int imgHeight = bmp.getHeight();
            int imgWidth = bmp.getWidth();

            //TRIM WIDTH - LEFT
            int startWidth = 0;
            for (int x = 0; x < imgWidth; x++) {
                if (startWidth == 0) {
                    for (int y = 0; y < imgHeight; y++) {
                        if (bmp.getPixel(x, y) != Color.WHITE) {
                            startWidth = x;
                            break;
                        }
                    }
                }
                else
                    break;
            }


            //TRIM WIDTH - RIGHT
            int endWidth = 0;
            for (int x = imgWidth - 1; x >= 0; x--) {
                if (endWidth == 0) {
                    for (int y = 0; y < imgHeight; y++) {
                        if (bmp.getPixel(x, y) != Color.WHITE) {
                            endWidth = x;
                            break;
                        }
                    }
                }
                else
                    break;
            }


            //TRIM HEIGHT - TOP
            int startHeight = 0;
            for (int y = 0; y < imgHeight; y++) {
                if (startHeight == 0) {
                    for (int x = 0; x < imgWidth; x++) {
                        if (bmp.getPixel(x, y) != Color.WHITE) {
                            startHeight = y;
                            break;
                        }
                    }
                }
                else
                    break;
            }


            //TRIM HEIGHT - BOTTOM
            int endHeight = 0;
            for (int y = imgHeight - 1; y >= 0; y--) {
                if (endHeight == 0) {
                    for (int x = 0; x < imgWidth; x++) {
                        if (bmp.getPixel(x, y) != Color.WHITE) {
                            endHeight = y;
                            break;
                        }
                    }
                }
                else
                    break;
            }


            return Bitmap.createBitmap(
                    bmp,
                    startWidth,
                    startHeight,
                    endWidth - startWidth + 1,
                    endHeight - startHeight + 1
            );
        }
        catch(Exception ex){
            //Quando ocorre um erro, ocorre pelo falo do mesmo não tem áreas vazias em algum canto
            return bmp;
        }
    }
}
