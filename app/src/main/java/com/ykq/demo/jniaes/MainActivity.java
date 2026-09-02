package com.ykq.demo.jniaes;

import android.app.Activity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

/**
 * create by yekangqi
 * <hr>
 * time: 2026/09/02 09:23
 * description: AES JNI 加解密演示页面
 */
public class MainActivity extends Activity implements View.OnClickListener {

    private EditText mEtEncrypt;
    private EditText mEtDecrypt;
    private TextView mTvDecryptResult;
    private TextView mTvCheck;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }

    private void initView() {
        mEtEncrypt = findViewById(R.id.et_encrypt);
        Button btnEncrypt = findViewById(R.id.btn_encrypt);
        mEtDecrypt = findViewById(R.id.et_decrypt);
        Button btnDecrypt = findViewById(R.id.btn_decrypt);
        mTvDecryptResult = findViewById(R.id.tv_decrypt_result);
        mTvCheck = findViewById(R.id.tv_check);

        btnEncrypt.setOnClickListener(this);
        btnDecrypt.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.btn_encrypt) {
            encryptInput();
        } else if (view.getId() == R.id.btn_decrypt) {
            decryptInput();
        }
    }

    private void encryptInput() {
        String input = mEtEncrypt.getText().toString();
        mEtDecrypt.setText(getAESEncrypt(input));
        mTvDecryptResult.setText("");
        mTvCheck.setText("");
    }

    private void decryptInput() {
        String encrypted = mEtDecrypt.getText().toString().trim();
        if (TextUtils.isEmpty(encrypted)) {
            Toast.makeText(this, R.string.error_empty_ciphertext, Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            String result = getAESDecrypt(encrypted);
            mTvDecryptResult.setText(result);
            boolean matches = mEtEncrypt.getText().toString().equals(result);
            mTvCheck.setText(matches ? R.string.check_success : R.string.check_failure);
        } catch (IllegalArgumentException exception) {
            mTvDecryptResult.setText("");
            mTvCheck.setText("");
            Toast.makeText(this, exception.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    public native String getAESEncrypt(String str);

    public native String getAESDecrypt(String str);

    static {
        System.loadLibrary("JNIAESDemo");
    }
}
