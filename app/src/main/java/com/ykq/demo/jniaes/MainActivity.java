package com.ykq.demo.jniaes;

import android.app.Activity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;


public class MainActivity extends Activity implements View.OnClickListener {

    private EditText mEtEncrpt;
    private Button mBtnEncrpt;
    private EditText mEtDecrypt;
    private Button mBtnDecrypt;
    private TextView mTvDecryptResult;
    private TextView mTvCheck;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.setContentView(R.layout.activity_main);
        this.initView();

    }

    private void initView() {
        this.mEtEncrpt = (EditText) this.findViewById(R.id.et_encrpt);
        this.mBtnEncrpt = (Button) this.findViewById(R.id.btn_encrpt);
        this.mEtDecrypt = (EditText) this.findViewById(R.id.et_decrypt);
        this.mBtnDecrypt = (Button) this.findViewById(R.id.btn_decrypt);
        this.mTvDecryptResult = (TextView) this.findViewById(R.id.tv_decrypt_result);
        this.mTvCheck = (TextView) this.findViewById(R.id.tv_check);

        this.mBtnEncrpt.setOnClickListener(this);
        this.mBtnDecrypt.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_encrpt:
                String input=this.mEtEncrpt.getText().toString();
                if (TextUtils.isEmpty(input))
                {
                    mEtDecrypt.setText("");
                    mTvCheck.setText("");
                    Toast.makeText(this,mEtEncrpt.getHint().toString(),Toast.LENGTH_SHORT).show();
                }else
                {
                    this.mEtDecrypt.setText(this.getAESEncrypt(input));
                }
                break;
            case R.id.btn_decrypt:
                String result= this.getAESDecrypt(this.mEtDecrypt.getText().toString()).trim();
                this.mTvDecryptResult.setText(result);
                this.mTvCheck.setText(this.mEtEncrpt.getText().toString().equals(result)?"成功!与加密前一致":"失败!与加密前不一致");
                break;
        }
    }
    public native String getAESEncrypt(String str);
    public native String getAESDecrypt(String str);
    static {
        System.loadLibrary("JNIAESDemo");
    }
}
