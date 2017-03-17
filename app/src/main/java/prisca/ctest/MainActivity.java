package prisca.ctest;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import uk.co.senab.photoview.PhotoViewAttacher;

import static prisca.ctest.ScanActivity.mBitmap;

public class MainActivity extends AppCompatActivity {

    public final static int OPEN_CAMERA = 4;
    public final static String OPEN_INTENT_PREFERENCE = "selectContent";
    public final static String SCANNED_RESULT = "scannedResult";
    private static final String TAG = "OCVSample::Activity";
    int REQUEST_CODE = 99;
    PhotoViewAttacher mAttacher;
    private ImageView scannedImageView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActivityCompat.requestPermissions(MainActivity.this,
                new String[]{Manifest.permission.CAMERA},
                1);

        scannedImageView = (ImageView) findViewById(R.id.imageview);
        mAttacher = new PhotoViewAttacher(scannedImageView);

        findViewById(R.id.goCamera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, ScanActivity.class);
                intent.putExtra(OPEN_INTENT_PREFERENCE, OPEN_CAMERA);
                startActivityForResult(intent, REQUEST_CODE);
            }
        });

        findViewById(R.id.goScan).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, OpenCvCamera.class));
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 1: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay! Do the
                    // contacts-related task you need to do.
                } else {
                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                    Toast.makeText(MainActivity.this, "Permission denied to read your External storage", Toast.LENGTH_SHORT).show();
                }
                return;
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            try {

                scannedImageView.setImageBitmap(mBitmap);
                // If you later call mImageView.setImageDrawable/setImageBitmap/setImageResource/etc then you just need to call
                mAttacher.update();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
