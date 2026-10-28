import torch

def generate_example_image():
    """Generates the base 2D grayscale image containing a square object."""
    img = torch.zeros((200, 200))
    # Synthetic square object located from x:50->150, y:50->150
    img[50:150, 50:150] = 1.0
    return img

def simulate_detector_predictions():
    """
    Simulates raw object detector outputs. 
    Returns overlapping bounding boxes around the square and their confidence scores.
    Format: [x1, y1, x2, y2]
    """
    boxes = torch.tensor([
        [48.0, 48.0, 152.0, 152.0],  # Slightly loose box, high score
        [50.0, 50.0, 150.0, 150.0],  # Perfect ground truth match, highest score
        [52.0, 49.0, 148.0, 151.0],  # Slightly shifted box, medium score
        [30.0, 30.0, 70.0, 70.0],    # False positive background patch, low score
        [45.0, 45.0, 155.0, 155.0],  # Very loose box, medium-low score
    ], dtype=torch.float32)
    
    scores = torch.tensor([0.91, 0.98, 0.84, 0.35, 0.62], dtype=torch.float32)
    return boxes, scores

def calculate_iou(boxes1: torch.Tensor, boxes2: torch.Tensor) -> torch.Tensor:
    """Computes pairwise Intersection over Union (IoU) between two sets of boxes."""
    # Broadcasting coordinates: (N, 1, 4) vs (1, M, 4)
    x1 = torch.max(boxes1[:, 0].unsqueeze(1), boxes2[:, 0].unsqueeze(0))
    y1 = torch.max(boxes1[:, 1].unsqueeze(1), boxes2[:, 1].unsqueeze(0))
    x2 = torch.min(boxes1[:, 2].unsqueeze(1), boxes2[:, 2].unsqueeze(0))
    y2 = torch.min(boxes1[:, 3].unsqueeze(1), boxes2[:, 3].unsqueeze(0))
    
    # Intersection area
    intersection = torch.clamp(x2 - x1, min=0) * torch.clamp(y2 - y1, min=0)
    
    # Individual areas
    area1 = (boxes1[:, 2] - boxes1[:, 0]) * (boxes1[:, 3] - boxes1[:, 1])
    area2 = (boxes2[:, 2] - boxes2[:, 0]) * (boxes2[:, 3] - boxes2[:, 1])
    
    # Union area = Area1 + Area2 - Intersection
    union = area1.unsqueeze(1) + area2.unsqueeze(0) - intersection
    
    # Numerical stability division
    return intersection / torch.clamp(union, min=1e-6)

def non_max_suppression(boxes: torch.Tensor, scores: torch.Tensor, iou_threshold: float) -> torch.Tensor:
    """
    Applies Non-Maximum Suppression to filter redundant overlapping boxes.
    Returns the indices of the kept bounding boxes.
    """
    if boxes.numel() == 0:
        return torch.empty((0,), dtype=torch.long)
    
    # Sort boxes by confidence score in descending order
    order = torch.argsort(scores, descending=True)
    keep = []
    
    while order.numel() > 0:
        # Pick the index of the box with the highest remaining score
        idx = order[0].item()
        keep.append(idx)
        
        if order.numel() == 1:
            break
            
        # Compute IoU between the top box and all remaining boxes
        top_box = boxes[idx].unsqueeze(0)
        remaining_boxes = boxes[order[1:]]
        iou = calculate_iou(top_box, remaining_boxes).squeeze(0)
        
        # Keep only boxes that have an IoU strictly below the threshold
        mask = iou <= iou_threshold
        order = order[1:][mask]
        
    return torch.tensor(keep, dtype=torch.long)

def run_pipeline():
    # 1. Initialize image context
    img_tensor = generate_example_image()
    print(f"Loaded synthetic image shape: {list(img_tensor.shape)}")
    
    # 2. Get simulated detector outputs
    boxes, scores = simulate_detector_predictions()
    print(f"Initial raw predictions: {boxes.shape[0]} bounding boxes detected.\n")
    
    # 3. Apply NMS with a threshold of 0.5
    iou_threshold = 0.5
    kept_indices = non_max_suppression(boxes, scores, iou_threshold)
    
    # 4. Output Results
    final_boxes = boxes[kept_indices]
    final_scores = scores[kept_indices]
    
    print(f"--- NMS Results (Threshold: {iou_threshold}) ---")
    print(f"  Surviving Boxes Count: {final_boxes.shape[0]}")
    for i, (box, score) in enumerate(zip(final_boxes, final_scores)):
        print(f"  Box {i+1}: Coords = {box.tolist()}, Score = {score.item():.2f}")

if __name__ == "__main__":
    run_pipeline()