using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace NumberRecognizer
{
    public partial class MainForm : Form
    {
        private bool onDraw;
        private List<Point> points;
        private Point lastLocation;
        private SolidBrush currentBrush;
        private Pen currentPen;
        private Bitmap canvas;
        private Graphics gdi;

        public MainForm()
        {
            InitializeComponent();
            canvas = new Bitmap(Canvas.ClientSize.Width, Canvas.ClientSize.Height);
            gdi = Graphics.FromImage(canvas);
            gdi.SmoothingMode = SmoothingMode.HighQuality;
            lastLocation = new Point(0, 0);
            currentBrush = new SolidBrush(Color.Black);
            currentPen = new Pen(currentBrush, 5)
            {
                EndCap = LineCap.Round,
                StartCap = LineCap.Round
            };
            onDraw = false;
            points = new List<Point>();
        }

        private void RecognizeBtn_Click(object sender, EventArgs e)
        {
            using (var bmp = new Bitmap(canvas.Width, canvas.Height))
            {
                using (var graphics = Graphics.FromImage(bmp))
                {
                    PointF centerOfMass = new PointF(0F, 0F);

                    foreach (var point in points)
                    {
                        centerOfMass.X += point.X;
                        centerOfMass.Y += point.Y;
                    }

                    centerOfMass.X /= points.Count;
                    centerOfMass.Y /= points.Count;

                    var center = new PointF(canvas.Width / 2F, canvas.Height / 2F);
                    var offset = new PointF(center.X - centerOfMass.X, center.Y - centerOfMass.Y);

                    for (int i = 0; i < points.Count; ++i)
                    {
                        points[i] = new Point((int)(points[i].X + offset.X), (int)(points[i].Y + offset.Y));
                    }

                    graphics.Clear(Color.White);
                    graphics.DrawLines(currentPen, points.ToArray());

                    var tmp = ResizeImage(bmp, 28, 28);
                    tmp.Save("C:\\Users\\dimni\\Desktop\\img.png", ImageFormat.Png);
                }
            }   
        }

        private void ClearBtn_Click(object sender, EventArgs e)
        {
            gdi.Clear(Color.White);
            points.Clear();
            Canvas.Image = canvas;
        }

        private void Canvas_MouseDown(object sender, MouseEventArgs e)
        {
            onDraw = true;
            gdi.Clear(Color.White);
            Canvas.Image = canvas;
            points.Add(e.Location);
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (onDraw)
            {
                points.Add(e.Location);
                gdi.DrawLines(currentPen, points.ToArray());
                Canvas.Image = canvas;
            }
        }

        private void Canvas_MouseUp(object sender, MouseEventArgs e)
        {
            onDraw = false;
        }

        private void PenThicknessNumber_ValueChanged(object sender, EventArgs e)
        {
            currentPen.Width = (float)((NumericUpDown)sender).Value;
        }

        private static Bitmap ResizeImage(Bitmap image, int width, int height)
        {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage))
            {
                graphics.CompositingMode    = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode  = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode      = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode    = PixelOffsetMode.HighQuality;

                using (var wrapMode = new ImageAttributes())
                {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }
    }
}
