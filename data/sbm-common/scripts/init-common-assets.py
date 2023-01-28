print("== Beginning: data/sbm-common/scripts/init-common-assets.py")

###  Assumes current directory is: libs/smartbody/sbm/bin
scene.addAssetPath('motion', 'sbm-common/common-sk')
# scene.addAssetPath('motion', 'data/retarget')

print(">>> Loading common motions and poses...")
scene.loadAssets()
print("Completed: sbm-common/scripts/init-common-assets.py")
