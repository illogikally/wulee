using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace Data_Hiding
{
    class BinaryImage
    {
        public static byte[,] K=new byte[3,3] {{0,1,0},{1,1,1},{0,1,0}}; // ma tran khoa K
        public static int sumK = 5;                                      // tong cac phan tu 1 trong K
        #region Bit Operator
        private static byte extract(byte n,byte pos) // trich 1 bit o vi tri pos tu byte n
        {
            return (byte)((n & (1 << pos))>>pos);
        }

        private static void replace(ref byte n, byte b,byte pos)       // thay the bit b vao vi tri thu 0 cua byte n
        {            
            n= (byte) (b==1 ? n|(1<<pos) : n&(~(1<<pos)));
        }
        #endregion
        private static void WuLee(ref byte[,] I,int m,int n,byte[] data)
        {
            if ((m * n / data.Length) < 9)
                MessageBox.Show("Dữ liệu quá lớn", "Error");

            int index = 0;  //bien chi so chay trong mang data[]
            for (int i = 0; ;i++)
            {                
                // lay ra 1 khoi F
                byte[,] F=new byte[3,3];
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        F[ii, jj] = I[(9 * i+3*ii+jj) / n, (9 * i+3*ii+jj) % n];    // cho nay rat kho hieu, dung co doc
                    }
                }
                // thuc hien toan tu F AND K va tinh sum(T)
                int sumT = 0;
                byte[,] T = new byte[3, 3];
                for (int ii = 0; ii < 3; ii++)
                    for (int jj = 0; jj < 3; jj++)
                    {
                        T[ii, jj] = (byte)(F[ii, jj] & K[ii, jj]);
                        sumT += T[ii, jj];
                    }
                #region Xet cac truong hop cua sumT
                if ((sumT > 0) && (sumT < sumK))    // truong hop sumT=0 (co the rat nhieu bit 0) hoac sumT=sumK (co the rat nhieu bit 1) khong duoc dung de giau tin vi rat de bi lo
                {                   
                    if (sumT % 2 != data[index])    // truong hop sumT%2==b thi khong can lam gi ca, do khoi F da dat bat bien
                    {
                        if (sumT == 1)  // khoi F nhieu bit 0 nen chon bit 0 de dao bit se it bi lo
                        {
                            // chon ngau nhien F[j,k]==0 va K[j,k]==1
                            int j=0, k=0;
                            for (int ii = 0; ii < 3; ii++)
                                for (int jj = 0; jj < 3; jj++)
                                    if (F[ii, jj] == 0 && K[ii, jj] == 1)
                                    {
                                        j = ii; k = jj; break;
                                    }
                            // dua F[j,k] ve 1
                            I[(9 * i + 3 * j + k) / n, (9 * i + 3 * j + k) % n] = 1;
                        }
                        else
                            if (sumT == sumK - 1)   // khoi F nhieu bit 1 nen chon bit 1 de dao se it bi lo
                            {
                                // chon ngau nhien F[j,k]==1 va K[j,k]==1
                                int j = 0, k = 0;
                                for (int ii = 0; ii < 3; ii++)
                                    for (int jj = 0; jj < 3; jj++)
                                        if (F[ii, jj] == 1 && K[ii, jj] == 1)
                                        {
                                            j = ii; k = jj; break;
                                        }
                                // dua F[j,k] ve 1
                                I[(9 * i + 3 * j + k) / n, (9 * i + 3 * j + k) % n] = 0;
                            }
                            else
                            {
                                // chon ngau nhien K[j,k]==1 va dao bit F[j,k]
                                int j = 0, k = 0;
                                for (int ii = 0; ii < 3; ii++)
                                    for (int jj = 0; jj < 3; jj++)
                                        if (K[ii, jj] == 1)
                                        {
                                            j = ii; k = jj; break;
                                        }
                                // dao bit F[j,k]
                                I[(9 * i + 3 * j + k) / n, (9 * i + 3 * j + k) % n] =(byte) (1 - I[(9 * i + 3 * j + k) / n, (9 * i + 3 * j + k) % n]);
                            }
                    }                
                    // kiem tra xem da giau het so bit chua
                    if (++index == data.Length) break;  //thoat khoi vong lap
                }
                #endregion
            }        
        }

        public static byte[,] encode(byte[,] image,int height,int width,byte[] hiddenData)
        {
            byte[,] I = new byte[height, width];    // anh nhi phan (0,1)
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    I[y,x] = extract(image[y,x], 0);        //LSB o cho nay
                }
            }
            // thuc hien giau tin tren ma tran nhi phan I[m,n]
            // dua mang byte ve mang bit
            byte[] data = new byte[hiddenData.Length * 8];  // 1 byte dung 8 bit
            
            for (int i = 0; i < hiddenData.Length; i++)
            {
                for (byte j = 0; j < 8; j++)
                {
                    byte b = extract(hiddenData[i], j);
                    data[8 * i + j] = b;
                }                   
            }
            WuLee(ref I, height, width, data);   
            // cap nhat lai
            
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    replace(ref image[y, x], I[y, x], 0);
                }
            }          
            return image;
        }

        private static byte[] readByte(byte[,] I, int m, int n, int k, ref int pos)// doc vao k byte tu vi tri pos
        {
            byte[] retArr = new byte[k];    // mang tra ve
            int j = 0;                      // chay trong mang retArr
            byte index = 0;                 // chay trong tung byte
            for (int i = pos; ; i++)
            {
                // lay ra 1 khoi F
                byte[,] F = new byte[3, 3];
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        F[ii, jj] = I[(9 * i + 3 * ii + jj) / n, (9 * i + 3 * ii + jj) % n];
                    }
                }
                // thuc hien toan tu F AND K va tinh sum(T)
                int sumT = 0;
                byte[,] T = new byte[3, 3];
                for (int ii = 0; ii < 3; ii++)
                    for (int jj = 0; jj < 3; jj++)
                    {
                        T[ii, jj] = (byte)(F[ii, jj] & K[ii, jj]);
                        sumT += T[ii, jj];
                    }
                if ((sumT <= 0) || (sumT >= sumK)) // khoi nay k duoc dung de giau tin
                {
                }
                else
                {
                    replace(ref retArr[j], (byte)(sumT % 2), index++);
                    if (index == 8)
                    {
                        j++;        //sang byte tiep theo
                        index = 0;  // bat dau doi voi 1 byte khac
                        if (j == k) // da doc du k byte
                        {
                            pos = i + 1;
                            break;  // te thoi
                        }
                    }
                }
            }
            return retArr;
        }
       

        public static string decode(byte[,] image, int m, int n, byte[] key)
        {
            //lay ra anh nhi phan I tu image
            byte[,] I = new byte[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    I[i, j] = extract(image[i, j], 0);
                }
            }
            int pos = 0;  // vi tri ban dau
            // kiem tra xem anh co chua tin an hay khong

            byte[] check = new byte[1];
            check = readByte(I, m, n, 1, ref pos); // doc lay 1 byte
                 
            if (check[0] != 117)
            {
                MessageBox.Show("Anh khong chua tin giau");
                Application.Exit();
            }

            // lay ra 2 byte do dai cua data[]
            byte[] temp = new byte[2];

            temp = readByte(I, m, n, 2, ref pos);

            Int16 len = BitConverter.ToInt16(temp, 0);
            //doc 'len' byte du lieu da giau
            byte[] data = new byte[len];
            data = readByte(I, m, n, len, ref pos);

            // thuc hien Xor data voi key[]
            for (int i = 0; i < data.Length; i++)
            {
                int index = i % key.Length;
                data[i] = (byte)(data[i] ^ key[index]);
            }
            UnicodeEncoding unicode=new UnicodeEncoding();
            return unicode.GetString(data);
        }    
    }
}
