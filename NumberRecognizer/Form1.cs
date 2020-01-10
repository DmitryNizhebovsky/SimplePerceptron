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
        private readonly List<Point> points;
        private readonly List<List<Point>> lines;
        private Point lastLocation;
        private SolidBrush currentBrush;
        private readonly Pen currentPen;
        private readonly Bitmap canvas;
        private readonly Graphics gdi;

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
            lines = new List<List<Point>>();
        }

        private void RecognizeBtn_Click(object sender, EventArgs e)
        {
            using (var bmp = new Bitmap(canvas.Width, canvas.Height))
            {
                using (var graphics = Graphics.FromImage(bmp))
                {
                    PointF centerOfMass = new PointF(0F, 0F);
                    var pointsCount = 0;

                    foreach (var line in lines)
                    {
                        foreach (var point in line)
                        {
                            centerOfMass.X += point.X;
                            centerOfMass.Y += point.Y;
                        }

                        pointsCount += line.Count;
                    }

                    centerOfMass.X /= pointsCount;
                    centerOfMass.Y /= pointsCount;

                    var center = new PointF(canvas.Width / 2F, canvas.Height / 2F);
                    var offset = new PointF(center.X - centerOfMass.X, center.Y - centerOfMass.Y);

                    foreach (var line in lines)
                    {
                        for (int i = 0; i < line.Count; ++i)
                        {
                            var newX = (int)(line[i].X + offset.X);
                            var newY = (int)(line[i].Y + offset.Y);
                            line[i] = new Point(newX, newY);
                        }
                    }

                    graphics.Clear(Color.White);

                    foreach (var line in lines)
                    {
                        graphics.DrawLines(currentPen, line.ToArray());
                    }

                    var tmp = ResizeImage(bmp, 28, 28);
                    var pixels = new List<float>();

                    for (var y = 0; y < tmp.Height; ++y)
                    {
                        for (var x = 0; x < tmp.Width; ++x)
                        {
                            var pixelColor = tmp.GetPixel(x, y);
                            var pixelGrey = (pixelColor.R + pixelColor.G + pixelColor.B) / 3F;
                            pixelGrey = (((255F - pixelGrey) / 255F) * 0.99F) + 0.01F;
                            pixels.Add(pixelGrey);
                        }
                    }
                }
            }   
        }

        private void ClearBtn_Click(object sender, EventArgs e)
        {
            points.Clear();
            lines.Clear();

            gdi.Clear(Color.White);
            Canvas.Image = canvas;
        }

        private void Canvas_MouseDown(object sender, MouseEventArgs e)
        {
            onDraw = true;
            points.Add(e.Location);
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (!onDraw) return;

            points.Add(e.Location);

            foreach (var line in lines)
            {
                gdi.DrawLines(currentPen, line.ToArray());
            }

            gdi.DrawLines(currentPen, points.ToArray());

            Canvas.Image = canvas;
        }

        private void Canvas_MouseUp(object sender, MouseEventArgs e)
        {
            onDraw = false;
            lines.Add(new List<Point>(points));
            points.Clear();
        }

        private void PenThicknessNumber_ValueChanged(object sender, EventArgs e)
        {
            currentPen.Width = (float)((NumericUpDown)sender).Value;
        }

        private static Bitmap ResizeImage(Image image, int width, int height)
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
