package org.binque.taaasty;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.CursorLoader;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.os.Build;



// http://hmkcode.com/android-display-selected-image-and-its-real-path/
public class RealPathUtil {
    public static String getPath(Context context, Intent data) {
       // SDK < API11
       if (Build.VERSION.SDK_INT < 11)
           return RealPathUtil.getRealPathFromURI_BelowAPI11(context, data.getData());

       // SDK >= 11 && SDK < 19
       if (Build.VERSION.SDK_INT < 19)
           return RealPathUtil.getRealPathFromURI_API11to18(context, data.getData());

       // SDK > 19 (Android 4.4)
       return RealPathUtil.getRealPathFromURI_API19(context, data.getData());
   }

    @SuppressLint("NewApi")
    public static String getRealPathFromURI_API19(Context context, Uri uri){
        String filePath = "";
        String wholeID = DocumentsContract.getDocumentId(uri);

         // Split at colon, use second item in the array
         String id = wholeID.split(":")[1];

         String[] column = { MediaStore.Images.Media.DATA };

         // where id is equal to
         String sel = MediaStore.Images.Media._ID + "=?";

         Cursor cursor = context.getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                                   column, sel, new String[]{ id }, null);

         int columnIndex = cursor.getColumnIndex(column[0]);

         if (cursor.moveToFirst()) {
             filePath = cursor.getString(columnIndex);
         }
         cursor.close();
         return filePath;
    }

    @SuppressLint("NewApi")
    public static String getRealPathFromURI_API11to18(Context context, Uri contentUri) {
          String[] proj = { MediaStore.Images.Media.DATA };
          String result = null;

          CursorLoader cursorLoader = new CursorLoader(
                  context,
            contentUri, proj, null, null, null);
          Cursor cursor = cursorLoader.loadInBackground();

          if(cursor != null){
           int column_index =
             cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
           cursor.moveToFirst();
           result = cursor.getString(column_index);
          }
          return result;
    }

    public static String getRealPathFromURI_BelowAPI11(Context context, Uri contentUri){
        String[] proj = { MediaStore.Images.Media.DATA };
        Cursor cursor = context.getContentResolver().query(contentUri, proj, null, null, null);
        int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        cursor.moveToFirst();
        return cursor.getString(column_index);
    }
}
