namespace kristal {
    [__AttributeUsage(_AttributeTargets.Function)]
    struct ExportAsset {
        String target;
    }
    [__AttributeUsage(_AttributeTargets.Function)]
    struct ExportCompute {
        String target;
    }

    void doNothing() {}
}
// TODO: Remove these duplicates when slang gets it's act together
[__AttributeUsage(_AttributeTargets.Function)]
struct kristal_ExportAsset {
    String target;
}
[__AttributeUsage(_AttributeTargets.Function)]
struct kristal_ExportCompute {
    String target;
}

