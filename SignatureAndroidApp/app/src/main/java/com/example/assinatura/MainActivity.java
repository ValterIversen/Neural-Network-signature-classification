package com.example.assinatura;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.content.SharedPreferences;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class MainActivity extends AppCompatActivity {

    final static String path = Environment.getExternalStorageDirectory().getPath() + "/UserSignature/";
    final static String TAG = TrainningActivity.class.getName();

    Button signatureButton, trainningButton, signUpButton;
    TextView lblInfo;
    EditText txtTrainning, txtSignUp;
    SharedPreferences prefs = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        signatureButton = (Button) findViewById(R.id.getSign);
        signUpButton = (Button) findViewById(R.id.getSignUp);
        trainningButton = (Button) findViewById(R.id.getTraining);
        lblInfo = (TextView) findViewById(R.id.tvInfo);
        txtTrainning = (EditText) findViewById(R.id.txtTrainning);
        txtSignUp = (EditText) findViewById(R.id.txtSignUp);
        signatureButton.setOnClickListener(onButtonClick);
        signUpButton.setOnClickListener(onButtonClick);
        trainningButton.setOnClickListener(onButtonClick);
        //to get imagepath from SignatureActivity and set it on ImageView
        String image_path = getIntent().getStringExtra("imagePath");
        String information = getIntent().getStringExtra("lblInfo");
        Bitmap bitmap = BitmapFactory.decodeFile(image_path);
        lblInfo.setText(information);
        prefs = getSharedPreferences("com.mycompany.myAppName", MODE_PRIVATE);

        if(isStoragePermissionGranted()){
            File file = new File(path);
            if (!file.exists()) {
                file.mkdir();
            }
        }
    }
    public static boolean saveToFile(String data, String fileName, boolean Append){
        try {
            new File(path).mkdir();
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

    public static  String ReadFile(Context context, String fileName){
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

    @Override
    protected void onResume() {
        super.onResume();

        File fileT = new File(path + "/TrainingData/authorTraining.txt");
        if(fileT.exists()){
            String authorFile = ReadFile(getApplicationContext(), "/TrainingData/authorTraining.txt").replace("\n", "");
            String[] fileText = authorFile.split(";");
            if(Integer.parseInt(fileText[1]) <= 10){
                txtTrainning.setText(fileText[0]);
                txtTrainning.setEnabled(false);
            }
            else{
                txtTrainning.setText("");
                txtTrainning.setEnabled(true);
            }
        }
        File fileS = new File(path + "/SignUp/authorSignUp.txt");
        if(fileS.exists()){
            String authorFile = ReadFile(getApplicationContext(), "/SignUp/authorSignUp.txt").replace("\n", "");
            String[] fileText = authorFile.split(";");
            if(Integer.parseInt(fileText[1]) <= 10){
                txtSignUp.setText(fileText[0]);
                txtSignUp.setEnabled(false);
            }
            else{
                txtSignUp.setText("");
                txtSignUp.setEnabled(true);
            }
        }
        else{
            txtSignUp.setText("");
            txtSignUp.setEnabled(true);
        }

        //Method to create Directory, if the Directory doesn't exists
        /*file = new File(DIRECTORY);
        if (!file.exists()) {
            signatureButton.setVisibility(View.GONE);
            trainningButton.setVisibility(View.VISIBLE);
        }
        else{
            trainningButton.setVisibility(View.GONE);
            signatureButton.setVisibility(View.VISIBLE);
        }*/
    }

    Button.OnClickListener onButtonClick = new Button.OnClickListener() {
        @Override
        public void onClick(View v) {



            // TODO Auto-generated method stub
            if (v.getId() == R.id.getSign) {
                Intent i = new Intent(MainActivity.this, LoginActivity.class);
                startActivity(i);
            }
            else if (v.getId() == R.id.getSignUp) {
                if((txtSignUp.getText() + "").trim() != "") {
                    File file = new File(path + "/SignUp/authorSignUp.txt");
                    if(!file.exists()){
                        if((txtSignUp.getText() + "").trim() != "")
                            saveToFile(txtSignUp.getText() + ";" + 1, "/SignUp/authorSignUp.txt", false);
                    }
                    else {
                        String authorFile = ReadFile(getApplicationContext(), "/SignUp/authorSignUp.txt").replace("\n", "");
                        String[] fileText = authorFile.split(";");
                        if (Integer.parseInt(fileText[1]) > 10) {
                            if((txtSignUp.getText() + "").trim() != "")
                                saveToFile(txtSignUp.getText() + ";" + 1, "/SignUp/authorSignUp.txt", false);
                        }
                    }
                    Intent i = new Intent(MainActivity.this, SignUpActivity.class);
                    startActivity(i);
                }
            }else if (v.getId() == R.id.getTraining){
                if((txtTrainning.getText() + "").trim() != "") {
                    File file = new File(path + "/TrainingData/authorTraining.txt");
                    if(!file.exists()){
                        if((txtTrainning.getText() + "").trim() != "")
                            saveToFile(txtTrainning.getText() + ";" + 1, "/TrainingData/authorTraining.txt", false);
                    }
                    else {
                        String authorFile = ReadFile(getApplicationContext(), "/TrainingData/authorTraining.txt").replace("\n", "");
                        String[] fileText = authorFile.split(";");
                        if (Integer.parseInt(fileText[1]) > 10) {
                            if((txtTrainning.getText() + "").trim() != "")
                                saveToFile(txtTrainning.getText() + ";" + 1, "/TrainingData/authorTraining.txt", false);
                        }
                    }
                    Intent i = new Intent(MainActivity.this, TrainningActivity.class);
                    startActivity(i);
                }
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
            File file = new File(path);
            if (!file.exists()) {
                file.mkdir();
            }
        }
        else
        {
            Toast.makeText(this, "The app was not allowed to write to your storage. Hence, it cannot function properly. Please consider granting it this permission", Toast.LENGTH_LONG).show();
        }
    }
}