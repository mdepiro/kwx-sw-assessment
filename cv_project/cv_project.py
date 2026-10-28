import torch
import numpy as np 
import matplotlib.pyplot as plt 
from PIL import Image

class imageClass:
    def __init__(self):
        self.test_image = self.generate_example_image()


    def generate_example_image(self):
        """Generates a synthetic 2d grayscale image for testing"""
        img = torch.zeros((200, 200))
        # Add a white square
        img[50:150, 50:150] = 1.0
        # Add some diagonal stripes
        for i in range(200):
            for j in range(200):
                if (i + j) % 30 == 0:
                    img[i, j] = 0.5
        return img

    def compute_image_metrics(self, tensor: torch.Tensor, name: str):
        """Computes and prints statistical tensor metrics for verification."""
        mean_val = tensor.mean().item()
        std_val = tensor.std().item()
        max_val = tensor.max().item()
        min_val = tensor.min().item()
        l2_norm = torch.linalg.vector_norm(tensor).item()
        
        print(f"--- {name} ---")
        print(f"  Shape:           {list(tensor.shape)}")
        print(f"  Mean Intensity:  {mean_val:.4f}")
        print(f"  Std Deviation:   {std_val:.4f}")
        print(f"  Min / Max Val:   {min_val:.4f} / {max_val:.4f}")
        print(f"  L2 Energy Norm:  {l2_norm:.4f}\n")

    def run_evaluation(self):
        # 1. Generate source image and shape for PyTorch convolutions
        # PyTorch conv2d expects 4D tensor: (Batch, Channels, Height, Width)
        self.test_image = img_tensor
        img_batched = img_tensor.unsqueeze(0).unsqueeze(0)  # Shape: (1, 1, 200, 200)
        
        compute_image_metrics(img_tensor, "Original Image Baseline")
        
        # 2. Define Kernels (Shape: Out_Channels, In_Channels, kH, kW)
        # Box Blur Kernel (5x5)
        blur_kernel = torch.ones((1, 1, 5, 5)) / 25.0
        
        # Vertical Sobel Edge Detector (3x3)
        sobel_kernel = torch.tensor([[[-1.0, -2.0, -1.0],
                                    [ 0.0,  0.0,  0.0],
                                    [ 1.0,  2.0,  1.0]]])
        sobel_kernel = sobel_kernel.unsqueeze(0)  # Shape: (1, 1, 3, 3)
        
        # 3. Apply Convolutions using PyTorch operations (with 'same' padding equivalent)
        blurred_tensor = F.conv2d(img_batched, blur_kernel, padding=2).squeeze()
        edge_tensor = F.conv2d(img_batched, sobel_kernel, padding=1).squeeze()
        
        # 4. Output Quantitative Metrics
        compute_image_metrics(blurred_tensor, "Filtered Output: Box Blur")
        compute_image_metrics(edge_tensor, "Filtered Output: Vertical Edges (Sobel)")

    
if __name__ == "__main__":
    pre_processor = imageClass()

    pre_processor.run_evaluation()