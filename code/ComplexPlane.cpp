#include "ComplexPlane.h"

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight) {
	m_pixel_size = Vector2i(pixelWidth, pixelHeight);
	m_aspectRatio = static_cast<float>(pixelHeight) / pixelWidth;
	m_plane_center = Vector2f(0, 0);
	m_plane_size = Vector2f(BASE_WIDTH, BASE_HEIGHT * m_aspectRatio);
	m_zoomCount = 0;
	m_state = State::CALCULATING;
	m_vArray.setPrimitiveType(Points);
	m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const {
	target.draw(m_vArray);
}

void ComplexPlane::updateRender() {
	if (m_state == CALCULATING) {

		int chunkSize = m_pixel_size.y / THREAD_COUNT;

		std::vector<std::thread> t;

		for (int i = 0; i < THREAD_COUNT; i++) {
			cout << "Creating thread" << endl;
			t.emplace_back([this, chunkSize, i] {
				this->updateRenderMultiThread(chunkSize, i);
			});
		}

		for (auto& th : t) {
			th.join();
		}

		m_state = State::DISPLAYING;
	}
}

void ComplexPlane::zoomIn() {
	m_zoomCount++;
	float x = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float y = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = Vector2f(x, y);
	m_state = CALCULATING;
}

void ComplexPlane::zoomOut() {
	m_zoomCount--;
	float x = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float y = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = Vector2f(x, y);
	m_state = CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel) {
	m_plane_center = mapPixelToCoords(mousePixel);
	cout << "New plane center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
	m_state = CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel) {
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text) {
	ostringstream output;
	output << "Mandelbrot Set\n" <<
		"Center: (" << m_plane_center.x << "," << m_plane_center.y << ")\n" <<
		"Cursor: (" << m_mouseLocation.x << "," << m_mouseLocation.y << ")\n" <<
		"Left-click to Zoom in\n" <<
		"Right-click to Zoom out\n";

	text.setString(output.str());
}

void ComplexPlane::updateRenderMultiThread(int chunkSize, int thread) {
	int startRow = chunkSize * thread;
	int endRow = (thread == THREAD_COUNT - 1) ? m_pixel_size.y : startRow + chunkSize;

	for (int i = startRow; i < endRow; i++) {
		for (int j = 0; j < m_pixel_size.x; j++) {
			m_vArray[j + i * m_pixel_size.x].position = { static_cast<float>(j), static_cast<float>(i) };
			size_t count = countIterations(mapPixelToCoords(Vector2i(j, i)));
			Uint8 r, g, b;
			iterationsToRGB(count, r, g, b);
			m_vArray[j + i * m_pixel_size.x].color = { r,g,b };
		}
	}
}

size_t ComplexPlane::countIterations(Vector2f coord) { 
	size_t i = 0;
	complex<double> c(coord.x, coord.y);
	complex<double> z(coord.x, coord.y);
	
	while (abs(z) < 2.0 && i < 64) {
		z = z * z + c;
		i++;
	}

	return i;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b) {
	if (count == MAX_ITER) {
		r = 0;
		g = 0;
		b = 0; // black
	}
	else if (count >= 0 && count < 6) {
		r = 123;
		g = 0;
		b = 255; // purple
	}
	else if (count >= 6 && count < 12) { //todo
		r = 61;
		g = 117;
		b = 255;
	}
	else if (count >= 12 && count < 18) {
		r = 0;
		g = 234;
		b = 255; // turquoise
	}
	else if (count >= 18 && count < 24) {
		r = 0;
		g = 245;
		b = 134;
	}
	else if (count >= 24 && count < 30) {
		r = 0;
		g = 255;
		b = 13; // green
	}
	else if (count >= 30 && count < 36){
		r = 128;
		g = 253;
		b = 7;
	}
	else if (count >= 36 && count < 42) {
		r = 255;
		g = 251;
		b = 0; // yellow
	}
	else if (count >= 42 && count < 48) {
		r = 255;
		g = 175;
		b = 0;
	}
	else if (count >= 48 && count << 54) {
		r = 255;
		g = 88;
		b = 0;
	}
	else {
		r = 255;
		g = 0;
		b = 0; // red
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel) {
	// a =
	// ((n-a) / (b-a)) * (d-c) + c
	float x = (static_cast<float>(mousePixel.x) / 1920) * m_plane_size.x + (m_plane_center.x - m_plane_size.x / 2.0);
	float y = ((static_cast<float>(mousePixel.y) - 1080) / (-1080)) * m_plane_size.y + (m_plane_center.y - m_plane_size.y / 2.0);

	return Vector2f(x, y);
}
