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
 * description: AES JNI 加解密与边界测试演示页面
 */
public class MainActivity extends Activity implements View.OnClickListener {

    private EditText mEtEncrypt;
    private EditText mEtDecrypt;
    private TextView mTvDecryptResult;
    private TextView mTvCheck;
    private TextView mTvTestResult;
    private int mTestPassed;
    private int mTestTotal;
    private StringBuilder mTestFailures;

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
        Button btnRunTests = findViewById(R.id.btn_run_tests);
        mTvDecryptResult = findViewById(R.id.tv_decrypt_result);
        mTvCheck = findViewById(R.id.tv_check);
        mTvTestResult = findViewById(R.id.tv_test_result);

        btnEncrypt.setOnClickListener(this);
        btnDecrypt.setOnClickListener(this);
        btnRunTests.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.btn_encrypt) {
            encryptInput();
        } else if (view.getId() == R.id.btn_decrypt) {
            decryptInput();
        } else if (view.getId() == R.id.btn_run_tests) {
            runCommonTests();
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

    private void runCommonTests() {
        mTestPassed = 0;
        mTestTotal = 0;
        mTestFailures = new StringBuilder();

        int[] boundaryLengths = {0, 1, 15, 16, 17, 31, 32, 33, 255, 256, 257, 4095, 4096, 4097};
        for (int length : boundaryLengths) {
            String input = createAsciiText(length);
            runTestCase("ASCII 长度 " + length + " 字节", () -> assertRoundTrip(input));
        }

        String[] numericBoundaries = {
                String.valueOf(Integer.MIN_VALUE),
                "-1",
                "0",
                "1",
                String.valueOf(Integer.MAX_VALUE),
                String.valueOf(Long.MIN_VALUE),
                String.valueOf(Long.MAX_VALUE)
        };
        for (String input : numericBoundaries) {
            runTestCase("数值边界 " + input, () -> assertRoundTrip(input));
        }

        runTestCase("中文 UTF-8", () -> assertRoundTrip("中文加解密：常见字符边界"));
        runTestCase("emoji 补充字符", () -> assertRoundTrip("emoji：😀🚀✅"));
        runTestCase("空格、制表符和换行", () -> assertRoundTrip("  前后空格\t\n第二行  "));
        runTestCase("内嵌空字符", () -> assertRoundTrip("A" + (char) 0 + "B"));
        runTestCase("类似填充的控制字符", () -> assertRoundTrip(createPaddingLikeText()));

        runTestCase("固定兼容向量", () -> {
            assertTextEquals("qkrxxA9fIF636aITDRJhcg==", getAESEncrypt("1"));
            assertTextEquals("1", getAESDecrypt("qkrxxA9fIF636aITDRJhcg=="));
        });
        runTestCase("ECB 相同输入结果一致", () -> {
            String first = getAESEncrypt("相同输入");
            String second = getAESEncrypt("相同输入");
            assertTextEquals(first, second);
        });

        runTestCase("加密参数为 null", () -> assertIllegalArgument(() -> getAESEncrypt(null)));
        runTestCase("解密参数为 null", () -> assertIllegalArgument(() -> getAESDecrypt(null)));
        runTestCase("空 Base64 密文", () -> assertIllegalArgument(() -> getAESDecrypt("")));
        runTestCase("非法 Base64 字符", () -> assertIllegalArgument(() -> getAESDecrypt("not-base64")));
        runTestCase("密文不足一个 AES 分组", () -> assertIllegalArgument(() -> getAESDecrypt("AAAA")));
        runTestCase("错误 PKCS#7 填充", () ->
                assertIllegalArgument(() -> getAESDecrypt("AAAAAAAAAAAAAAAAAAAAAA==")));
        runTestCase("非规范 Base64 填充位", () -> assertIllegalArgument(() -> getAESDecrypt("AB==")));

        String summary = getString(R.string.test_result_summary, mTestPassed, mTestTotal);
        if (mTestFailures.length() == 0) {
            mTvTestResult.setText(getString(R.string.test_result_success, summary));
        } else {
            mTvTestResult.setText(getString(
                    R.string.test_result_failure, summary, mTestFailures.toString()));
        }
        Toast.makeText(this, summary, Toast.LENGTH_SHORT).show();
    }

    private void runTestCase(String name, TestAction action) {
        mTestTotal++;
        try {
            action.run();
            mTestPassed++;
        } catch (RuntimeException | AssertionError error) {
            String message = TextUtils.isEmpty(error.getMessage())
                    ? error.getClass().getSimpleName()
                    : error.getMessage();
            mTestFailures.append("\n- ").append(name).append("：").append(message);
        }
    }

    private void assertRoundTrip(String input) {
        String encrypted = getAESEncrypt(input);
        if (TextUtils.isEmpty(encrypted) || encrypted.length() % 4 != 0) {
            throw new AssertionError("加密结果不是有效长度的 Base64 文本");
        }
        assertTextEquals(input, getAESDecrypt(encrypted));
    }

    private void assertIllegalArgument(TestAction action) {
        try {
            action.run();
        } catch (IllegalArgumentException exception) {
            if (TextUtils.isEmpty(exception.getMessage())) {
                throw new AssertionError("异常提示为空");
            }
            return;
        }
        throw new AssertionError("未抛出 IllegalArgumentException");
    }

    private void assertTextEquals(String expected, String actual) {
        if (!expected.equals(actual)) {
            throw new AssertionError("加解密结果不一致");
        }
    }

    private String createAsciiText(int length) {
        StringBuilder builder = new StringBuilder(length);
        for (int i = 0; i < length; i++) {
            builder.append((char) ('A' + i % 26));
        }
        return builder.toString();
    }

    private String createPaddingLikeText() {
        StringBuilder builder = new StringBuilder(16);
        for (int value = 1; value <= 16; value++) {
            builder.append((char) value);
        }
        return builder.toString();
    }

    private interface TestAction {
        void run();
    }

    public native String getAESEncrypt(String str);

    public native String getAESDecrypt(String str);

    static {
        System.loadLibrary("JNIAESDemo");
    }
}
