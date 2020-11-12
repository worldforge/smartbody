print("|-------------------------------------------------|")
print("|  data/sbm-common/scripts/characterTest.py     |")
print("|-------------------------------------------------|")
import time


def testRetargetCharacter(charName):
    # test gesture
    sbChar = scene.getCharacter(charName)
    skelName = sbChar.getSkeleton().getName()
    sbChar.setBoolAttribute("steering.pathFollowingMode", True)
    bml.execBML(charName, '<body posture="' + skelName + 'HandsAtSide_Motex"/>')

    bml.execBML(charName, '<animation name="' + skelName + 'HandsAtSide_RArm_GestureYou"/>')

    # test head nod
    bml.execBML(charName, '<head type="NOD" start="2"/>')
    bml.execBML(charName, '<head type="SHAKE" start="4"/>')
    bml.execBML(charName, '<head type="TOSS" start="6"/>')
    bml.execBML(charName, '<animation name="' + skelName + 'HandsAtSide_RArm_GestureYou"/>')
    # test gaze
    pawnTargetName = charName + "target"
    testPawn = scene.getPawn(pawnTargetName)
    if (testPawn == None):
        testPawn = scene.createPawn(pawnTargetName);
    position = sbChar.getPosition()
    print('sbChar.getPosition=' + str(position.getData(0)) + ' ,' + str(position.getData(1)) + ', ' + str(position.getData(2)))
    gazePos = SrVec(position.getData(0), sbChar.getHeight() * 1.8, position.getData(2) + sbChar.getHeight() * 0.6);
    print('sbChar.getPosition=' + str(gazePos.getData(0)) + ' ,' + str(gazePos.getData(1)) + ', ' + str(gazePos.getData(2)))
    testPawn.setPosition(gazePos)
    bml.execBML(charName, '<gaze target="' + pawnTargetName + '" sbm:joint-speed="5200" start="8"/>')

    # gazePos.setData(1,0.0)
    # gazePawn.setPosition(gazePos)
    scene.commandAt(10.0, 'set pawn ' + pawnTargetName + ' world_offset y 0')
    # bml.execBML(charName, '<gaze target="'+ gazeTargetName+ '"/>')
    scene.commandAt(12.0, 'char ' + charName + ' gazefade out 1.5')

    # test point
    pointPos = SrVec(position.getData(0) - sbChar.getHeight() * 0.2, sbChar.getHeight() * 2.15, position.getData(2) + sbChar.getHeight() * 1.15);
    scene.commandAt(14.0, 'set pawn ' + pawnTargetName + ' world_offset x ' + str(pointPos.getData(0)) + ' y ' + str(pointPos.getData(1)) + ' z ' + str(
        pointPos.getData(2)))
    scene.commandAt(14.1, 'bml char ' + charName + ' <sbm:reach sbm:action="point-at" sbm:reach-duration="0.5" target="' + pawnTargetName + '"/>')

    pointPos = SrVec(position.getData(0) - sbChar.getHeight() * 1.0, sbChar.getHeight() * 0.0, position.getData(2) + sbChar.getHeight() * 1.1);
    scene.commandAt(18.0, 'set pawn ' + pawnTargetName + ' world_offset x ' + str(pointPos.getData(0)) + ' y ' + str(pointPos.getData(1)) + ' z ' + str(
        pointPos.getData(2)))
    scene.commandAt(18.1, 'bml char ' + charName + ' <sbm:reach sbm:action="point-at" sbm:reach-duration="0.5" target="' + pawnTargetName + '"/>')

    pointPos = SrVec(position.getData(0) - sbChar.getHeight() * 0.2, sbChar.getHeight() * 0.5, position.getData(2) + sbChar.getHeight() * 2.1);
    scene.commandAt(22.0, 'set pawn ' + pawnTargetName + ' world_offset x ' + str(pointPos.getData(0)) + ' y ' + str(pointPos.getData(1)) + ' z ' + str(
        pointPos.getData(2)))
    scene.commandAt(22.1, 'bml char ' + charName + ' <sbm:reach sbm:action="point-at" sbm:reach-duration="0.5" target="' + pawnTargetName + '"/>')
    # test reach
    reachPos = SrVec(position.getData(0) - sbChar.getHeight() * 0.2, sbChar.getHeight() * 0.8, position.getData(2) + sbChar.getHeight() * 0.2);
    print('sbChar.getPosition=' + str(reachPos.getData(0)) + ' ,' + str(reachPos.getData(1)) + ', ' + str(reachPos.getData(2)))
    scene.commandAt(26.0, 'set pawn ' + pawnTargetName + ' world_offset x ' + str(reachPos.getData(0)) + ' y ' + str(reachPos.getData(1)) + ' z ' + str(
        reachPos.getData(2)))
    scene.commandAt(26.1, 'bml char ' + charName + ' <sbm:reach sbm:action="touch" sbm:reach-duration="0.5" target="' + pawnTargetName + '"/>')
    # bml.execBML(charName, '<sbm:reach sbm:action="touch" sbm:reach-duration="0.5" target="'+pawnTargetName+'" start="14"/>')

    reachPos = SrVec(position.getData(0) - sbChar.getHeight() * 0.2, sbChar.getHeight() * 0.15, position.getData(2) + sbChar.getHeight() * 0.15);
    scene.commandAt(30.0, 'set pawn ' + pawnTargetName + ' world_offset x ' + str(reachPos.getData(0)) + ' y ' + str(reachPos.getData(1)) + ' z ' + str(
        reachPos.getData(2)))
    scene.commandAt(30.1, 'bml char ' + charName + ' <sbm:reach sbm:action="touch" sbm:reach-duration="0.5" target="' + pawnTargetName + '"/>')

    # /bml.execBML(charName, '<sbm:reach sbm:action="touch" sbm:reach-duration="0.5" target="'+pawnTargetName+'" start="18"/>')
    bml.execBML(charName, '<animation name="' + skelName + 'LHandOnHip_Arms_GestureWhy" start="35"/>')
    # test locomotion
    bml.execBML(charName, '<locomotion target="' + str(position.getData(0) - sbChar.getHeight() * 0.1) + ' ' + str(
        position.getData(2) - sbChar.getHeight() * 3) + '" start="37"/>')
    # scene.removePawn(pawnTargetName)
    bml.execBML(charName, '<locomotion target="' + str(position.getData(0)) + ' ' + str(position.getData(2)) + '" start="45" facing="0"/>')
