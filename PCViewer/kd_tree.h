#ifndef kd_tree_H
#define kd_tree_H
#include <vector>

class KdTree {
public:
	enum BoundsBehaviour {
		KdTree_Bounds_Static,				//no border adjustment
		KdTree_Bounds_Pull_In_Outer_Border,	//pull in the outer border at splits
		KdTree_Bounds_Pull_In_Both_Borders	//pull in bot borders
	};

	KdTree() {};
	KdTree(std::vector<int>& indices, std::vector<float*>& data, std::vector<int>& attributes, std::vector<std::pair<float, float>> initialBounds, int recursionDepth, BoundsBehaviour adjustBounds) {
		//building the kd tree;
		this->adjustBounds = adjustBounds;
		this->attributes = attributes;
		nodes.reserve(pow(2,recursionDepth + 1)-1);
		root = buildRec(0, indices, data, attributes, initialBounds, recursionDepth);
	};
	~KdTree() {};

	std::vector<std::vector<std::pair<float, float>>> getBounds(int recursionDepth) {
		return getBoundsRec(root, recursionDepth);
	};

private:
	struct Node {
		int split;
		std::vector<std::pair<float, float>> bounds;
		Node* leftChild;
		Node* rightChild;
	};

	//contains all nodes
	std::vector<Node> nodes;
	std::vector<int> attributes;
	Node* root;
	BoundsBehaviour adjustBounds;

	Node* buildRec(int split, std::vector<int>& indices, std::vector<float*>& data, std::vector<int> attributes, std::vector<std::pair<float,float>>& bounds, int recDepth) {
		if (!indices.size() || !recDepth) return nullptr;
		Node n = {};
		n.bounds = bounds;
		n.split = split;

		//splitting the bounding box in the middle
		std::vector<std::pair<float, float>> leftBounds(bounds), rightBounds(bounds);
		float mid = (leftBounds[split].first + leftBounds[split].second) / 2;
		leftBounds[split].second = mid;
		rightBounds[split].first = mid;
		switch (adjustBounds) {
		case KdTree_Bounds_Static: break;
		case KdTree_Bounds_Pull_In_Outer_Border: 
			leftBounds[split].first = leftBounds[split].second;
			rightBounds[split].second = rightBounds[split].first;
			break;
		case KdTree_Bounds_Pull_In_Both_Borders:
			float tmp = leftBounds[split].first;
			leftBounds[split].first = leftBounds[split].second;
			leftBounds[split].second = tmp;
			tmp = rightBounds[split].first;
			rightBounds[split].first = rightBounds[split].second;
			rightBounds[split].second = tmp;
			break;
		}

		//assining the points to the left and right bounding box. also get the maximum extent of the points in both directions if wanted
		std::vector<int> leftPts, rightPts;
		for (int i : indices) {
			float val = data[i][attributes[split]];
			if (val < mid) {
				switch (adjustBounds) {
				case KdTree_Bounds_Static: break;
				case KdTree_Bounds_Pull_In_Outer_Border:
					if (val < leftBounds[split].first) leftBounds[split].first = val;
					break;
				case KdTree_Bounds_Pull_In_Both_Borders:
					if (val < leftBounds[split].first) leftBounds[split].first = val;
					if (val > leftBounds[split].second) leftBounds[split].second = val;
					break;
				}
				leftPts.push_back(i);
			}
			else {
				switch (adjustBounds) {
				case KdTree_Bounds_Static: break;
				case KdTree_Bounds_Pull_In_Outer_Border:
					if (val > rightBounds[split].second) rightBounds[split].second = val;
					break;
				case KdTree_Bounds_Pull_In_Both_Borders:
					if (val > rightBounds[split].second) rightBounds[split].second = val;
					if (val < rightBounds[split].first) rightBounds[split].first = val;
					break;
				}
				rightPts.push_back(i);
			}
		}
		
		//creating the childs recursiveley
		int s2 = (split + 1) % attributes.size();
		n.leftChild = buildRec(s2, leftPts, data, attributes, leftBounds, recDepth -1);
		n.rightChild = buildRec(s2, rightPts, data, attributes, rightBounds, recDepth -1);
		nodes.push_back(n);
		return &nodes.back();
	};

	std::vector<std::vector<std::pair<float,float>>> getBoundsRec(Node* n, int recDepth) {
		if (!recDepth) {
			std::vector<std::vector<std::pair<float,float>>> r;
			r.push_back(n->bounds);
			return r;
		}

		//getting the bounds from the left and right child and appending the vectors
		std::vector<std::vector<std::pair<float, float>>> left = (n->leftChild)?getBoundsRec(n->leftChild, recDepth - 1):std::vector<std::vector<std::pair<float, float>>>(), right = (n->rightChild)?getBoundsRec(n->rightChild, recDepth - 1): std::vector<std::vector<std::pair<float, float>>>();
		left.reserve(left.size() + right.size());
		left.insert(left.end(), right.begin(), right.end());
		return left;
	};
};


#endif // !kd_tree_H
