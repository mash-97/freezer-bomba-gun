
int randInt(int min, int max) {
	return min + rand() % (max-min+1);
}

float randFloat(float min, float max) {
	return min + (float)rand() / RAND_MAX * (max-min);
}
